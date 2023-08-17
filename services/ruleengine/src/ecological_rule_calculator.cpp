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

#include "ecological_rule_calculator.h"
#include "var_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE_BASE"
EcologicalRuleCalculator *EcologicalRuleCalculator::ecologicalRuleCalculator =
    new (std::nothrow) EcologicalRuleCalculator();

EcologicalRuleCalculator::EcologicalRuleCalculator() {}

EcologicalRuleCalculator::~EcologicalRuleCalculator() {}

EcologicalRuleCalculator *EcologicalRuleCalculator::GetInstance()
{
    return ecologicalRuleCalculator;
}

BaseExp *EcologicalRuleCalculator::AnalysisDsl(const std::string expression)
{
    std::stack<BaseExp *> expStack;
    try {
        int32_t ret = TraversalDslExpression(expression, expStack);
        if (ret != SUCCESS_RULE) {
            throw nullptr;
        }
    } catch (...) {
        LOG_ERROR("[AnalysisDsl] fail:catch exception expression is %{public}s.", expression.c_str());
        ClearExpStack(expStack);
        return nullptr;
    }
    if (expStack.size() != 1) {
        LOG_ERROR("[AnalysisDsl] fail:expression stack is error expression is %{public}s.", expression.c_str());
        ClearExpStack(expStack);
        return nullptr;
    }
    return expStack.top();
}

int32_t EcologicalRuleCalculator::TraversalDslExpression(const std::string expression, std::stack<BaseExp *> &expStack)
{
    size_t pos = 0;
    BaseExp *exp = nullptr;
    std::string subString = "";
    std::stack<std::string> operatorStack;

    for (size_t i = 0; i < expression.size(); ++i) {
        if (!IsSeparator(expression[i])) {
            continue;
        }
        subString = expression.substr(pos, i - pos);
        StrTrimSpace(subString);

        if (expression[i] == LEFT_PARENTHESIS) {
            operatorStack.push(subString);
        } else if (expression[i] == COMMA && i != 0) {
            if (expression[i - 1] == RIGHT_PARENTHESIS) {
                pos = i + 1;
                continue;
            }
            exp = GetVarExp(subString);
            if (exp == nullptr) {
                LOG_ERROR("[TraversalDslExpression] fail: get var expression failed.");
                return ERROR_RULE;
            }
            expStack.push(exp);
        } else {
            exp = GetCalculatorExp(subString, operatorStack, expStack);
            if (exp == nullptr) {
                LOG_ERROR("[TraversalDslExpression] fail:get calculator expression failed.");
                return ERROR_RULE;
            }
            expStack.push(exp);
        }
        pos = i + 1;
    }
    return SUCCESS_RULE;
}

void EcologicalRuleCalculator::ClearExpStack(std::stack<BaseExp *> &expStack)
{
    BaseExp *topExp = nullptr;
    while (!expStack.empty()) {
        topExp = expStack.top();
        delete topExp;
        expStack.pop();
    }
}

bool EcologicalRuleCalculator::IsSeparator(char character)
{
    if (character == LEFT_PARENTHESIS || character == RIGHT_PARENTHESIS || character == COMMA) {
        return true;
    }
    return false;
}

void EcologicalRuleCalculator::StrTrimSpace(std::string &str)
{
    if (str.empty()) {
        return;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
}

void EcologicalRuleCalculator::StrTrimDoubleQuote(std::string &str)
{
    if (str.empty()) {
        return;
    }
    str.erase(0, str.find_first_not_of("\""));
    str.erase(str.find_last_not_of("\"") + 1);
}

BaseExp *EcologicalRuleCalculator::GetVarExp(std::string var)
{
    StrTrimDoubleQuote(var);
    if (var.empty()) {
        return nullptr;
    }

    return new VarExp(var);
}

BaseExp *EcologicalRuleCalculator::GetCalculatorExp(const std::string expression,
    std::stack<std::string> &operatorStack, std::stack<BaseExp *> &expStack)
{
    return nullptr;
}

// Get unary operator evaluation expression
BaseExp *EcologicalRuleCalculator::GetCalculatorExpforUnary(BaseExp *lastExp, OperatorInfo operatorInfo)
{
    return nullptr;
}

// Get binary operator evaluation expression
BaseExp *EcologicalRuleCalculator::GetCalculatorExpforBinary(BaseExp *lastExp, OperatorInfo operatorInfo,
    std::stack<BaseExp *> &expStack)
{
    return nullptr;
}

// Get ternary operator evaluation expression
BaseExp *EcologicalRuleCalculator::GetCalculatorExpforTernary(BaseExp *lastExp, OperatorInfo operatorInfo,
    std::stack<BaseExp *> &expStack)
{
    return nullptr;
}

int32_t EcologicalRuleCalculator::QueryMatchSceneExperience(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    const std::string experienceType, ExperienceRule &experienceRule)
{
    experienceRule.isAllow = true;
    experienceRule.sceneCode = "00000";
    return SUCCESS_RULE;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS