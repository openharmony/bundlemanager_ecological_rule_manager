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

#ifndef SERVICES_INCLUDE_ECOLOGICAL_RULE_CALCULATOR_IMPL_H
#define SERVICES_INCLUDE_ECOLOGICAL_RULE_CALCULATOR_IMPL_H

#include "ecological_rule_calculator.h"
namespace OHOS {
namespace EcologicalRuleMgrService {
// operator name
const std::string OPERATION_OR = "OR";
const std::string OPERATION_EQUAL = "EQUAL";
const std::string OPERATION_BUNDLENAME = "BUNDLENAME";
const std::string OPERATION_AND = "AND";
const std::string OPERATION_IS_JUMP_TYPE = "IS_JUMP_TYPE";
// operator id
const int32_t OR_ID = 1;
const int32_t EQUAL_ID = 2;
const int32_t BUNDLENAME_ID = 3;
const int32_t AND_ID = 4;
const int32_t IS_JUMP_TYPE_ID = 5;
// key word
const std::string KEYWORD_RULES = "RULELIST";
const std::string KEYWORD_LIST = "list";
const std::string KEYWORD_CONDITION = "condition";
const std::string KEYWORD_SCENECODE = "sceneCode";
const std::string KEYWORD_EXPERIENCE = "experience";

struct RuleStrategyObject {
    BaseExp *dslExp;
    std::string sceneCode;
    std::string experience;
};

class EcologicalRuleCalculatorImpl : public EcologicalRuleCalculator {
public:
    EcologicalRuleCalculatorImpl(const EcologicalRuleCalculatorImpl &other) = delete;
    EcologicalRuleCalculatorImpl &operator = (const EcologicalRuleCalculatorImpl &other) = delete;

    static EcologicalRuleCalculatorImpl *GetInstance();
    int32_t QueryMatchSceneExperience(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        const std::string experienceType, ExperienceRule &experienceRule);

    int32_t ConstructRuleStrategyObjects();

protected:
    BaseExp *GetCalculatorExp(const std::string expression, std::stack<std::string> &operatorStack,
        std::stack<BaseExp *> &expStack);
    BaseExp *GetCalculatorExpforUnary(BaseExp *lastExp, OperatorInfo operatorInfo);
    BaseExp *GetCalculatorExpforBinary(BaseExp *lastExp, OperatorInfo operatorInfo, std::stack<BaseExp *> &expStack);
    BaseExp *GetCalculatorExpforTernary(BaseExp *lastExp, OperatorInfo operatorInfo, std::stack<BaseExp *> &expStack);

private:
    static EcologicalRuleCalculatorImpl *ecologicalRuleCalculatorImpl;
    static const std::map<std::string, OperatorInfo> OPERATOR_STR_INFO_MAP;

    bool CalculateDslMatchResult(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        std::map<std::string, CalculatorResult> &presetCache, BaseExp *DslExp);

    std::mutex ruleStrategyObjectLock_;
    std::vector<RuleStrategyObject> ruleStrategyObjects;

    void GetRuleStrategyObjectLock();
    void ReleaseRuleStrategyObjectLock();
    void ClearRuleStrategyObjects();

    EcologicalRuleCalculatorImpl();
    ~EcologicalRuleCalculatorImpl();

    int32_t GetExperienceResult(nlohmann::json &experienceConfig, const std::string experienceType,
        ExperienceRule &experienceRule);
    static const std::map<std::string, std::string> EXPERIENCE_TYPE_MAP;
    std::string TransRealExperienceType(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        const std::string experienceType, std::map<std::string, CalculatorResult> &presetCache);
    int32_t GetOperatorInfo(std::stack<std::string> &operatorStack, OperatorInfo &operatorInfo, bool &hasOperator);
    BaseExp *GetLastExp(const std::string expression, std::stack<BaseExp *> &expStack);
    int32_t GetSceneExperienceFromCache(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
        const std::string transExperienceType, std::map<std::string, CalculatorResult> &presetCache,
        ExperienceRule &experienceRule);
    std::string GetSceneCodeBundleName(const OHOS::AAFwk::Want &want, const std::string sceneCode);
};
}
}
#endif