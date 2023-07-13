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

#include "or_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE"
OrExp::OrExp()
{
    leftChild = nullptr;
    rightChild = nullptr;
}

OrExp::OrExp(BaseExp *leftChild, BaseExp *rightChild) : leftChild(leftChild), rightChild(rightChild) {}

OrExp::~OrExp()
{
    delete leftChild;
    delete rightChild;
}

CalculatorResult OrExp::Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache)
{
    CalculatorResult calculatorResult;
    calculatorResult.type = BOOL_TYPE;
    calculatorResult.logicRet = false;

    try {
        CalculatorResult leftCalculatorResult = leftChild->Interpreter(want, callerInfo, presetCache);
        if (leftCalculatorResult.type != BOOL_TYPE) {
            LOG_ERROR("[OrExp] fail: leftchild result type is invalid.");
            throw std::runtime_error("left result is invalid");
        }

        if (leftCalculatorResult.logicRet == true) {
            calculatorResult.logicRet = true;
        } else {
            CalculatorResult rightCalculatorResult = rightChild->Interpreter(want, callerInfo, presetCache);
            if (rightCalculatorResult.type != BOOL_TYPE) {
                LOG_ERROR("[OrExp] fail: rightChild result type is valid.");
                throw std::runtime_error("right result is invalid");
            }
            if (rightCalculatorResult.logicRet == true) {
                calculatorResult.logicRet = true;
            }
        }
    } catch (...) {
        LOG_ERROR("[OrExp] fail: catch exception.");
    }

    return calculatorResult;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS