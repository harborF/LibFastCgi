#include "ModuleInitHandler.h"


namespace  Protruly {

    ModuleInitHandler::ModuleInitHandler(fastcgi::ComponentContext *context) :fastcgi::Component(context), logger_(NULL)
    {
    }

    ModuleInitHandler::~ModuleInitHandler() {

    }

    void ModuleInitHandler::onLoad() {
        const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
        logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
        if (!logger_) {
            throw std::runtime_error("cannot get component " + loggerComponentName);
        }
    }

    void ModuleInitHandler::onUnload() {

    }

    static std::string getYear() {
        time_t t = time(NULL);
        struct tm* ptm = localtime(&t);

        char szBuf[32];
        strftime(szBuf, 32, "%Y", ptm);
        return std::string(szBuf);
    }

    void ModuleInitHandler::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext)
    {

        const std::string& strMemId = req->getArg("member_id");
        const std::string& strMerchantId = req->getArg("merchant_id");


        req->setStatus(200);
        req->setContentType("text/plain;charset=utf-8");
        fastcgi::RequestStream stream(req);
        stream << "hello world";
    }

}//end namespace
