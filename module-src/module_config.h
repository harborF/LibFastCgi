#ifndef __M_CONFIG_HEAD__
#define __M_CONFIG_HEAD__
#include <stdexcept>

#include "fastcgi2/logger.h"
#include "fastcgi2/config.h"
#include "fastcgi2/RequestStream.h"
#include "fastcgi2/handler.h"
#include "fastcgi2/request.h"
#include "fastcgi2/component.h"
#include "fastcgi2/ComponentFactory.h"

//const length 
#define CACHE_DATA_COUNT 100
#define REDIS_EXPIRE_TIME (60*5)
#define TRANS_ID_LEN 19
#define BIND_CARD_MAX 10

#define DATA_MORE_NO 1
#define DATA_MORE_YES 0

#define REQ_FILE_LOCK_CONF_NAME "/fastcgi/daemon/file_lock" 
#define UNINX_REDIS_CONF_NAME  "/fastcgi/daemon/redis_path"

#define GRP_DB_IDX(n) static_cast<uint8_t>(n%10)
#define GRP_TAB_IDX(n) static_cast<uint8_t>(n/10%20)

#define MEM_DB_IDX(n) static_cast<uint8_t>(n%10)
#define MEM_TAB_IDX(n) static_cast<uint8_t>(n/10%20)

#endif
