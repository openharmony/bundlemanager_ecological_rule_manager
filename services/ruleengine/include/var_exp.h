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

#ifndef SERVICES_INCLUDE_VAR_EXP_H
#define SERVICES_INCLUDE_VAR_EXP_H

#include <string>
#include "base_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
class VarExp : public BaseExp {
public:
    VarExp(std::string varStr);
    ~VarExp();
    CalculatorResult Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        std::map<std::string, CalculatorResult> &presetCache);

private:
    VarExp();
    std::string varStr;
};
} // EcologicalRuleMgrService
} // OHOS

#endif // SERVICES_INCLUDE_VAR_EXP_H