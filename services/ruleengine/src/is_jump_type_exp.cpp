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

#include "is_jump_type_exp.h"
#include "ecological_rule_engine_cache.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
const std::string IsJumpTypeExp::OPERATION_NAME = "IS_JUMP_TYPE";
#define TAG "RULE_ENGINE"
IsJumpTypeExp::IsJumpTypeExp()
{
    leftChild = nullptr;
    middleChild = nullptr;
    rightChild = nullptr;
}

IsJumpTypeExp::IsJumpTypeExp(BaseExp *leftChild, BaseExp *middleChild, BaseExp *rightChild)
    : leftChild(leftChild), middleChild(middleChild), rightChild(rightChild)
{}

IsJumpTypeExp::~IsJumpTypeExp()
{
    delete leftChild;
    delete middleChild;
    delete rightChild;
}

CalculatorResult IsJumpTypeExp::Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache)
{
    CalculatorResult calculatorResult;
    calculatorResult.type = BOOL_TYPE;
    calculatorResult.logicRet = false;
    try {
        CalculatorResult leftCalculatorResult = leftChild->Interpreter(want, callerInfo, presetCache);
        if (leftCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[IsJumpTypeExp] fail: left Child result type is invalid.");
            throw std::runtime_error("IsJumpTypeExp left result is invalid");
        }

        CalculatorResult middleCalculatorResult = middleChild->Interpreter(want, callerInfo, presetCache);
        if (middleCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[IsJumpTypeExp] fail: middleChild result type is invalid.");
            throw std::runtime_error("IsJumpTypeExp middle result is invalid");
        }

        CalculatorResult rightCalculatorResult = rightChild->Interpreter(want, callerInfo, presetCache);
        if (rightCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[IsJumpTypeExp] fail: rightChild result type is invalid.");
            throw std::runtime_error("IsJumpTypeExp right result is invalid");
        }

        std::string cacheKey = OPERATION_NAME + leftCalculatorResult.stringRet + middleCalculatorResult.stringRet +
            rightCalculatorResult.stringRet;
        std::map<std::string, CalculatorResult>::iterator it = presetCache.find(cacheKey);
        if (it != presetCache.end()) {
            return it->second;
        }

        std::string jumpType = GetJumpType(want, callerInfo);
        if (jumpType.compare(rightCalculatorResult.stringRet) == 0) {
            calculatorResult.logicRet = true;
        }

        // set execution result to cache
        presetCache.insert(std::pair<std::string, CalculatorResult>(cacheKey, calculatorResult));
    } catch (...) {
        LOG_ERROR("[IsJumpTypeExp] fail: catch exception.");
    }

    return calculatorResult;
}

std::string IsJumpTypeExp::GetJumpType(const Want &want, const CallerInfo &callerInfo)
{
    std::string jumpType;
    if (callerInfo.targetAppType == CallerInfo::TYPE_ATOM_SERVICE) {
        jumpType = JUMP_TYPE_IN;
    } else if (callerInfo.callerAppType == CallerInfo::TYPE_ATOM_SERVICE) {
        jumpType = JUMP_TYPE_OUT;
    } else {
        jumpType = JUMP_TYPE_APPS;
    }

    LOG_DEBUG("callerType: %{public}d, targetType: %{public}d, JumpType: %{public}s.", callerInfo.callerAppType,
        callerInfo.targetAppType, jumpType.c_str());
    return jumpType;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS