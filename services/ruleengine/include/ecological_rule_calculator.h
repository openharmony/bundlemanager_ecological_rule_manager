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

#ifndef SERVICES_INCLUDE_ECOLOGICAL_RULE_CALCULATOR_H
#define SERVICES_INCLUDE_ECOLOGICAL_RULE_CALCULATOR_H

#include <map>
#include <string>
#include <stack>
#include <vector>

#include "base_exp.h"
#include "ecological_rule_mgr_service_param.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
const char LEFT_PARENTHESIS = '(';
const char RIGHT_PARENTHESIS = ')';
const char COMMA = ',';
const char LEFT_DOUBLE_QUOTATION = '\"';

// input experience type
const std::string EXPERIENCE_TYPE_LOAD = "load";
const std::string EXPERIENCE_TYPE_OPEN = "open";
const std::string EXPERIENCE_TYPE_OUT = "out";

enum OperatorType {
    UNARY_OPERATOR = 1,
    BINARY_OPERATOR = 2,
    TERNARY_OPERATOR = 3,
};

enum ReturnCode {
    ERROR_RULE = -1,
    SUCCESS_RULE = 0,
    NO_MATCH_RULE = 1,
};

struct OperatorInfo {
    int32_t operatorID;
    int32_t operatorType;
};

class EcologicalRuleCalculator {
public:
    EcologicalRuleCalculator(const EcologicalRuleCalculator &other) = delete;
    EcologicalRuleCalculator &operator = (const EcologicalRuleCalculator &other) = delete;

    static EcologicalRuleCalculator *GetInstance();
    virtual int32_t QueryMatchSceneExperience(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        const std::string experienceType, ExperienceRule &experienceRule);

protected:
    EcologicalRuleCalculator();
    virtual ~EcologicalRuleCalculator();

    bool IsSeparator(char character);
    void StrTrimSpace(std::string &str);
    void StrTrimDoubleQuote(std::string &str);

    BaseExp *AnalysisDsl(const std::string expression);
    BaseExp *GetVarExp(std::string var);
    void ClearExpStack(std::stack<BaseExp *> &expStack);
    virtual BaseExp *GetCalculatorExpforUnary(BaseExp *lastExp, OperatorInfo operatorInfo);
    virtual BaseExp *GetCalculatorExpforBinary(BaseExp *lastExp, OperatorInfo operatorInfo,
        std::stack<BaseExp *> &expStack);
    virtual BaseExp *GetCalculatorExpforTernary(BaseExp *lastExp, OperatorInfo operatorInfo,
        std::stack<BaseExp *> &expStack);
    virtual BaseExp *GetCalculatorExp(const std::string expression, std::stack<std::string> &operatorStack,
        std::stack<BaseExp *> &expStack);
    int32_t TraversalDslExpression(const std::string expression, std::stack<BaseExp *> &expStack);

private:
    static EcologicalRuleCalculator *ecologicalRuleCalculator;
};
}
}
#endif