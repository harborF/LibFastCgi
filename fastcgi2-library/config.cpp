#include "settings.h"

#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>

#include "fastcgi2/config.h"
#include "fastcgi2-ext/XmlConfig.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include <iostream>

namespace fastcgi
{
    Config::Config()
    {}

    Config::~Config() {
    }

    std::unique_ptr<Config> Config::create(const char *file) {
        return std::unique_ptr<Config>(new XmlConfig(file));
    }

    std::unique_ptr<Config> Config::create(int &argc, char *argv[], HelpFunc func) {
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "--config", sizeof("--config") - 1) == 0) {
                const char *pos = strchr(argv[i], '=');
                if (NULL != pos) {
                    std::unique_ptr<Config> conf(new XmlConfig(pos + 1));
                    std::swap(argv[i], argv[argc - 1]);
                    --argc;
                    return conf;
                }
            }
        }
        std::stringstream stream;
        if (NULL != func) {
            func(stream);
        }
        else {
            stream << "usage: fastcgi-daemon --config=<config file>";
        }
        throw std::logic_error(stream.str());
    }

    const std::string& Config::filename() const {
        return filename_;
    }

    void Config::setFilename(const std::string &name) {
        filename_ = name;
    }

    XmlConfig::XmlConfig(const char *file)
    {
        try {
            regex_.assign("\\$\\{([A-Za-z][A-Za-z0-9\\-]*)\\}");
        }
        catch (const std::regex_error& e) {
            throw std::runtime_error(std::string("XmlConfig: ").append(e.what()));
        }

        try {
            if (!doc_.load_file(file)){
                throw std::runtime_error(std::string("can not open ").append(file));
            }
            setFilename(file);

            if (doc_.empty()) {
                throw std::logic_error("got empty config");
            }
            this->findVariables();
        }
        catch (const std::ios::failure &e) {
            throw std::runtime_error(std::string("can not read ").append(file));
        }
    }

    XmlConfig::~XmlConfig() {
    }

    int XmlConfig::asInt(const std::string &key) const {
        return str_cast_i<int>(asString(key));
    }

    int XmlConfig::asInt(const std::string &key, int defval) const {
        try {
            return asInt(key);
        }
        catch (const std::exception &e) {
            return defval;
        }
    }

    std::string XmlConfig::asString(const std::string &key) const {

        std::string res;

        const pugi::xpath_node object = doc_.select_node(key.c_str());
        if (object.node().empty() && object.attribute().empty()) {
            throw std::runtime_error(std::string("nonexistent config param: ").append(key));
        }
        const char *val = object.node().empty() == false ? object.node().text().get() : object.attribute().value();
        if (NULL != val && strlen(val)) {
            res.assign(val);
        }

        resolveVariables(res);
        return res;
    }

    std::string XmlConfig::asString(const std::string &key, const std::string &defval) const {
        try {
            return asString(key);
        }
        catch (const std::exception &e) {
            return defval;
        }
    }

    void XmlConfig::subKeys(const std::string &key, std::vector<std::string> &v) const {

        std::string tmp;

        const pugi::xpath_node_set object = doc_.select_nodes(key.c_str());
        if (!object.empty()) {
            v.reserve(object.size());

            int i = 1;
            for (auto it = object.begin(); it != object.end(); ++it) {
                pugi::xpath_node node = *it;
                const char *name = node.node().name();
                if (NULL == name) {
                    throw std::logic_error("bad variable definition");
                }
                std::stringstream stream;
                stream << key << "[" << (i++) << "]";
                v.push_back(stream.str());
            }
        }//end if
    }

    void XmlConfig::findVariables() {

        pugi::xpath_node_set object = doc_.select_nodes("/fastcgi/variables/variable");
        if (object.empty()) {
            return;
        }

        for (pugi::xpath_node_set::const_iterator it = object.begin(); it != object.end(); ++it)
        {
            pugi::xpath_node node = *it;
            const char *val = node.node().text().get();
            const char *name = node.node().name();
            if (NULL == val || NULL == name) {
                throw std::logic_error("bad variable definition");
            }
            vars_.insert(std::pair<std::string, std::string>(name, val));
        }
    }

    void XmlConfig::resolveVariables(std::string &val) const {
        std::smatch res;
        while (std::regex_search(val, res, regex_)) {
            if (2 == res.size()) {
                std::string key(res[1].first, res[1].second);
                val.replace(res.position(static_cast<std::smatch::size_type>(0)), res.length(0), findVariable(key));
            }
        }
    }

    const std::string& XmlConfig::findVariable(const std::string &key) const {
        std::map<std::string, std::string>::const_iterator i = vars_.find(key);
        if (vars_.end() != i) {
            return i->second;
        }
        else {
            throw std::runtime_error(std::string("nonexistent variable ").append(key));
        }
    }

} // namespace fastcgi
