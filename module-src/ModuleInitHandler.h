#ifndef _M_INIT_HANDLER_H__
#define _M_INIT_HANDLER_H__
#include "module_config.h"

namespace  Protruly {

	class ModuleInitHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler
	{
	public:
        ModuleInitHandler(fastcgi::ComponentContext *context);
		virtual ~ModuleInitHandler();
		
		virtual void onLoad();
		virtual void onUnload();


		virtual void handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);

	private:
		fastcgi::Logger *logger_;
	};

}//end namespace

#endif
