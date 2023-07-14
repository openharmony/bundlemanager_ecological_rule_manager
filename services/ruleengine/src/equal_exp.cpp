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

#include "equal_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE"
EqualExp::EqualExp()
{
    leftChild = nullptr;
    rightChild = nullptr;
}

EqualExp::EqualExp(BaseExp *leftChild, BaseExp *rightChild) : leftChild(leftChild), rightChild(rightChild) {}

EqualExp::~EqualExp()
{
    delete leftChild;
    delete rightChild;
}

CalculatorResult EqualExp::Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache)
{
    CalculatorResult calculatorResult;
    calculatorResult.type = BOOL_TYPE;
    calculatorResult.logicRet = false;
    try {
        CalculatorResult leftCalculatorResult = leftChild->Interpreter(want, callerInfo, presetCache);
        if (leftCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[EqualExp] fail: leftchild result type is invalid.");
            throw std::runtime_error("left child result is invalid");
        }

        CalculatorResult rightCalculatorResult = rightChild->Interpreter(want, callerInfo, presetCache);
        if (rightCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[EqualExp] fail: rightChild result type is invalid.");
            throw std::runtime_error("right child result is invalid");
        }

        if (leftCalculatorResult.stringRet.compare(rightCalculatorResult.stringRet) == 0) {
            calculatorResult.logicRet = true;
        }
    } catch (...) {
        LOG_ERROR("[EqualExp] fail: catch exception.");
    }

    return calculatorResult;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS