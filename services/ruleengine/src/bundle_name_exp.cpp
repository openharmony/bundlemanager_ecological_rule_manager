/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.

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

#include "bundle_name_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE"
BundleNameExp::BundleNameExp()
{
    child = nullptr;
}

BundleNameExp::BundleNameExp(BaseExp *child) : child(child) {}

BundleNameExp::~BundleNameExp()
{
    delete child;
}

CalculatorResult BundleNameExp::Interpreter(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache)
{
    CalculatorResult calculatorResult;
    calculatorResult.type = STRING_TYPE;
    try {
        CalculatorResult childCalculatorResult = child->Interpreter(want, callerInfo, presetCache);
        if (childCalculatorResult.type != STRING_TYPE) {
            LOG_ERROR("[BundleNameExp] fail: child result type is invalid.");
            throw std::runtime_error("child result is invalid");
        }

        std::string keyWord = childCalculatorResult.stringRet;
        transform(keyWord.begin(), keyWord.end(), keyWord.begin(), ::tolower);

        if (keyWord.compare(KEYWORD_WANT) == 0) {
            calculatorResult.stringRet = want.GetBundle();
        } else {
            calculatorResult.stringRet = callerInfo.packageName;
        }
    } catch (...) {
        LOG_ERROR("[BundleNameExp] fail: catch exception.");
        calculatorResult.stringRet = "";
    }

    return calculatorResult;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS