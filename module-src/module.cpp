#include <iostream>
#include <stdexcept>

#include "ModuleInitHandler.h"

namespace Protruly {
	FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
		FCGIDAEMON_ADD_DEFAULT_FACTORY("init", ModuleInitHandler)
    FCGIDAEMON_REGISTER_FACTORIES_END()
}
