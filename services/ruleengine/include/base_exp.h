/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_INCLUDE_BASE_EXP_H
#define SERVICES_INCLUDE_BASE_EXP_H

#include <string>
#include <map>
#include <exception>
#include "ecological_rule_mgr_service_client.h"
#include "ecological_rule_mgr_service_logger.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace EcologicalRuleMgrService {
const std::string KEYWORD_WANT = "want";
const std::string KEYWORD_CALLER = "caller";

enum ExpReturnCode {
    SUCCESS = 0,
    ERROR = -1,
};

enum CalculatorResultType {
    NULL_TYPE = 0,
    BOOL_TYPE = 1,
    STRING_TYPE = 2,
    JSON_TYPE = 3,
};

struct CalculatorResult {
    CalculatorResultType type;
    bool logicRet;
    std::string stringRet;
    nlohmann::json *jsonRet;
};

class BaseExp {
public:
    BaseExp(){};
    virtual ~BaseExp(){};
    virtual CalculatorResult Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        std::map<std::string, CalculatorResult> &presetCache) = 0;

private:
};
} // EcologicalRuleMgrService
} // OHOS

#endif // SERVICES_INCLUDE_BASE_EXP_H