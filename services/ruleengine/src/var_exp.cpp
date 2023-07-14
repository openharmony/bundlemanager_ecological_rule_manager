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

#include "var_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE"
VarExp::VarExp() {}

VarExp::VarExp(std::string varStr) : varStr(varStr) {}

VarExp::~VarExp() {}

CalculatorResult VarExp::Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache)
{
    CalculatorResult calculatorResult;
    calculatorResult.type = STRING_TYPE;
    calculatorResult.stringRet = varStr;
    return calculatorResult;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS