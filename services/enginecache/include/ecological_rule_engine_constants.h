/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

const std::string KEYWORD_RULE_CONTROL_STRATEGY = "RULELIST";

// json key
const std::string APP_ENHANCE_DATA_KEY_DATA_VER = "dataVersion";

const std::string SCENE_DATA_KEY_CONFIG_VER = "configVersion";
const std::string SCENE_DATA_KEY_RULE_CONFIG = "ruleConfig";
const std::string SCENE_DATA_KEY_CARD_OWENERS = "addCardOwners";
const std::string SCENE_DATA_KEY_SCENE_EXP = "sceneExperience";
const std::string SCENE_EXPERIENCE_KEY_ALLOW = "allow";
const std::string SCENE_EXPERIENCE_KEY_WANT = "want";
const std::string CARD_OWENERS_KEY_PKG_NAME = "pkgName";

const std::string APP_STRATEGY_DATA_ROOT_KEY = "list";

// scene_code type
const int32_t SCENE_CODE_TYPE_COLD = 1;
const int32_t SCENE_CODE_TYPE_HOT = 2;

// timer
const uint64_t TIMER_INTERVAL_5S_MS = (uint64_t)5000;
const uint64_t TIMER_INTERVAL_60MIN_MS = (uint64_t)3600 * 1000;
const uint64_t TIMER_INTERVAL_30MIN_MS = (uint64_t)1800 * 1000;
const bool TIMER_NO_REPEAT_ = false;
const bool TIMER_REPEAT_ = true;
const int32_t TIMER_TYPE_PRECISION_ = (int32_t)4; // precision timer
} // namespace EngineCacheConstants
} // namespace EcologicalRuleMgrService
} // namespace OHOS
#endif
