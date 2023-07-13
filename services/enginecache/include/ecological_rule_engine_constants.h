/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: constant info about cache function
 */

#ifndef FOUNDATION_ECOLOGICAL_RULE_MGR_SERVICE_INCLUDE_ENGINE_CONSTANTS_H
#define FOUNDATION_ECOLOGICAL_RULE_MGR_SERVICE_INCLUDE_ENGINE_CONSTANTS_H

#include <string>

namespace OHOS {
namespace EcologicalRuleMgrService {
namespace EngineCacheConstants {
const int32_t APP_ENHANCED_DATA_ALL_ADD = 0;
const int32_t APP_ENHANCED_DATA_PART_ADD = 1;
const int32_t APP_ENHANCED_DATA_DEL = -1;

const std::string KEYWORD_APP_EXTENSION = "APPLIST";
const std::string KEYWORD_RULE_CONTROL_STRATEGY = "RULELIST";

// json key
const std::string APP_ENHANCE_DATA_ROOT_KEY = "list";
const std::string APP_ENHANCE_DATA_PKG_NAME = "packageName";
const std::string APP_ENHANCE_DATA_KEY_DATA_VER = "dataVersion";

const std::string SCENE_DATA_KEY_CONFIG_VER = "configVersion";
const std::string SCENE_DATA_KEY_RULE_CONFIG = "ruleConfig";
const std::string SCENE_DATA_KEY_ADS_INFOS = "adsInfos";
const std::string SCENE_DATA_KEY_CARD_OWENERS = "addCardOwners";
const std::string SCENE_DATA_KEY_SCENE_EXP = "sceneExperience";
const std::string SCENE_EXPERIENCE_KEY_ALLOW = "allow";
const std::string SCENE_EXPERIENCE_KEY_WANT = "want";
const std::string ADS_INFOS_KEY_PLATFORM_TYPE = "platformType";
const std::string ADS_INFOS_KEY_PUBLIC_KEY = "publicKey";
const std::string ADS_INFOS_KEY_AD_NETWORK_ID = "adNetworkId";
const std::string CARD_OWENERS_KEY_PKG_NAME = "pkgName";

const std::string APP_STRATEGY_DATA_ROOT_KEY = "list";

// ads platform
const int32_t ADS_INFOS_PLATFORM_TYPE_AGDS = 1;
const int32_t ADS_INFOS_PLATFORM_TYPE_PPS = 2;

// scene_code type
const int32_t SCENE_CODE_TYPE_COLD = 1;
const int32_t SCENE_CODE_TYPE_HOT = 2;

// file
const std::string FILE_STORE_DIR = "/data/service/el1/public/ecological_rule_mgr_service/";
const std::string SCENE_CONFIG_DATA_FILE = FILE_STORE_DIR + "scene_experience_data.json";
const std::string APP_RULE_STRATEGY_FILE = FILE_STORE_DIR + "rule_strategy_data.json";
const std::string APP_ENHANCE_DATA_FILE  = FILE_STORE_DIR + "app_enhance_data";
const std::string APP_ENHANCE_DATA_SEPARATOR  = ",";

const std::string APP_ENHANCE_OUT_FILE = FILE_STORE_DIR + "trace_app_enhance.json";
const std::string APP_RULE_OUT_FILE = FILE_STORE_DIR + "trace_rule_strategy.json";
const std::string SCENE_CONFIG_OUT_FILE = FILE_STORE_DIR + "trace_scene_experience.json";

// timer
const uint64_t TIMER_INTERVAL_5S_MS = (uint64_t)5000;
const uint64_t TIMER_INTERVAL_60MIN_MS = (uint64_t)3600 * 1000;
const uint64_t TIMER_INTERVAL_30MIN_MS = (uint64_t)1800 * 1000;
const uint64_t TIMER_INTERVAL_10MIN_MS = (uint64_t)600 * 1000;
const bool TIMER_NO_REPEAT_ = false;
const bool TIMER_REPEAT_ = true;
const int32_t TIMER_TYPE_PRECISION_ = (int32_t)4; // precision timer

// device type
const std::string DEVICETYPE_PHONE = "phone";
const std::string DEVICETYPE_TABLET = "tablet";

// config data type
const int32_t RULE_STRATEGY_DATA_TYPE = 1;
const int32_t APP_ENHANCE_DATA_TYPE = 2;
const int32_t SCENE_CONFIG_DATA_TYPE = 3;
} // namespace EngineCacheConstants
} // namespace EcologicalRuleMgrService
} // namespace OHOS
#endif
