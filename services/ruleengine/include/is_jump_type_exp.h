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

#ifndef SERVICES_INCLUDE_IS_JUMP_TYPE_EXP_H
#define SERVICES_INCLUDE_IS_JUMP_TYPE_EXP_H

#include "base_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
// jump type
const std::string JUMP_TYPE_OUT = "1";
const std::string JUMP_TYPE_IN = "2";
const std::string JUMP_TYPE_APPS = "3";

class IsJumpTypeExp : public BaseExp {
public:
    IsJumpTypeExp(BaseExp *leftChild, BaseExp *middleChild, BaseExp *rightChild);
    ~IsJumpTypeExp();
    CalculatorResult Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        std::map<std::string, CalculatorResult> &presetCache);
    static std::string GetJumpType(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo);

private:
    IsJumpTypeExp();

    BaseExp *leftChild;
    BaseExp *middleChild;
    BaseExp *rightChild;

    static const std::string OPERATION_NAME;
};
} // EcologicalRuleMgrService
} // OHOS

#endif // SERVICES_INCLUDE_IS_JUMP_TYPE_EXP_H