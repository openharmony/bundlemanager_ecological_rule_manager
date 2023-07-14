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
#include "ecological_rule_calculator_impl.h"
#include "ecological_rule_engine_cache.h"
#include "var_exp.h"
#include "or_exp.h"
#include "equal_exp.h"
#include "bundle_name_exp.h"
#include "and_exp.h"
#include "is_jump_type_exp.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "RULE_ENGINE_IMPL"
EcologicalRuleCalculatorImpl *EcologicalRuleCalculatorImpl::ecologicalRuleCalculatorImpl =
    new (std::nothrow) EcologicalRuleCalculatorImpl();

const std::map<std::string, OperatorInfo> EcologicalRuleCalculatorImpl::OPERATOR_STR_INFO_MAP {
    { OPERATION_OR, { OR_ID, BINARY_OPERATOR } },
    { OPERATION_EQUAL, { EQUAL_ID, BINARY_OPERATOR } },
    { OPERATION_BUNDLENAME, { BUNDLENAME_ID, UNARY_OPERATOR } },
    { OPERATION_AND, { AND_ID, BINARY_OPERATOR } },
    { OPERATION_IS_JUMP_TYPE, { IS_JUMP_TYPE_ID, TERNARY_OPERATOR } },
};

const std::map<std::string, std::string> EcologicalRuleCalculatorImpl::EXPERIENCE_TYPE_MAP {
    { EXPERIENCE_TYPE_LOAD, "1" },
    { EXPERIENCE_TYPE_OPEN, "2" },
    { EXPERIENCE_TYPE_OUT, "5" },
};

EcologicalRuleCalculatorImpl::EcologicalRuleCalculatorImpl() {}

EcologicalRuleCalculatorImpl::~EcologicalRuleCalculatorImpl() {}

EcologicalRuleCalculatorImpl *EcologicalRuleCalculatorImpl::GetInstance()
{
    return ecologicalRuleCalculatorImpl;
}

BaseExp *EcologicalRuleCalculatorImpl::GetCalculatorExp(const std::string expression,
    std::stack<std::string> &operatorStack, std::stack<BaseExp *> &expStack)
{
    BaseExp *exp = nullptr;
    BaseExp *outputExp = nullptr;
    try {
        OperatorInfo operatorInfo;
        bool hasOperator = false;
        int32_t ret = GetOperatorInfo(operatorStack, operatorInfo, hasOperator);
        if (ret != SUCCESS_RULE) {
            return nullptr;
        }
        // Obtains the last operation expression.
        exp = GetLastExp(expression, expStack);
        if (exp == nullptr) {
            return nullptr;
        }

        switch (operatorInfo.operatorType) {
            case UNARY_OPERATOR: {
                outputExp = GetCalculatorExpforUnary(exp, operatorInfo);
                break;
            }
            case BINARY_OPERATOR: {
                outputExp = GetCalculatorExpforBinary(exp, operatorInfo, expStack);
                break;
            }
            case TERNARY_OPERATOR: {
                outputExp = GetCalculatorExpforTernary(exp, operatorInfo, expStack);
                break;
            }
            default: {
                throw nullptr;
            }
        }

        if (outputExp == nullptr) {
            throw nullptr;
        }
    } catch (...) {
        LOG_ERROR("[GetCalculatorExp impl] fail: catch exception.");
        if (exp != nullptr) {
            delete exp;
        }
        if (outputExp != nullptr) {
            delete outputExp;
        }
        outputExp = nullptr;
    }
    return outputExp;
}

BaseExp *EcologicalRuleCalculatorImpl::GetLastExp(const std::string expression, std::stack<BaseExp *> &expStack)
{
    BaseExp *exp = GetVarExp(expression);
    if (exp == nullptr) {
        if (expStack.empty()) {
            return nullptr;
        }
        exp = expStack.top();
        if (exp == nullptr) {
            return nullptr;
        }
        expStack.pop();
    }
    return exp;
}

int32_t EcologicalRuleCalculatorImpl::GetOperatorInfo(std::stack<std::string> &operatorStack,
    OperatorInfo &operatorInfo, bool &hasOperator)
{
    // get operator info.
    if (operatorStack.empty()) {
        LOG_ERROR("[GetOperatorInfo impl] fail: operator stack is empty.");
        return ERROR_RULE;
    }

    std::string operatorStr = operatorStack.top();
    operatorStack.pop();

    std::map<std::string, OperatorInfo>::const_iterator it = OPERATOR_STR_INFO_MAP.find(operatorStr);
    if (it == OPERATOR_STR_INFO_MAP.end()) {
        LOG_ERROR("[GetOperatorInfo impl] fail: get operator info from string failed.");
        return ERROR_RULE;
    }
    operatorInfo = it->second;

    return SUCCESS_RULE;
}

// Get unary operator evaluation expression
BaseExp *EcologicalRuleCalculatorImpl::GetCalculatorExpforUnary(BaseExp *lastExp, OperatorInfo operatorInfo)
{
    switch (operatorInfo.operatorID) {
        case BUNDLENAME_ID: {
            return new BundleNameExp(lastExp);
        }
        default: {
            return nullptr;
        }
    }
}

// Get binary operator evaluation expression
BaseExp *EcologicalRuleCalculatorImpl::GetCalculatorExpforBinary(BaseExp *lastExp, OperatorInfo operatorInfo,
    std::stack<BaseExp *> &expStack)
{
    BaseExp *leftExp = nullptr;
    try {
        if (expStack.empty()) {
            LOG_ERROR("[GetCalculatorExpforBinary] fail: left expression statck is empty.");
            throw nullptr;
        }

        leftExp = expStack.top();
        if (leftExp == nullptr) {
            LOG_ERROR("[GetCalculatorExpforBinary] fail: get left expression failed.");
            throw nullptr;
        }
        expStack.pop();

        switch (operatorInfo.operatorID) {
            case OR_ID: {
                return new OrExp(leftExp, lastExp);
            }
            case EQUAL_ID: {
                return new EqualExp(leftExp, lastExp);
            }
            case AND_ID: {
                return new AndExp(leftExp, lastExp);
            }
            default: {
                throw nullptr;
            }
        }
    } catch (...) {
        LOG_ERROR("[GetCalculatorExpforBinary] fail: catch exception.");
        if (leftExp != nullptr) {
            delete leftExp;
        }
        return nullptr;
    }
}
// Get ternary operator evaluation expression
BaseExp *EcologicalRuleCalculatorImpl::GetCalculatorExpforTernary(BaseExp *lastExp, OperatorInfo operatorInfo,
    std::stack<BaseExp *> &expStack)
{
    BaseExp *middleExp = nullptr;
    BaseExp *leftExp = nullptr;
    try {
        // get middle expression
        if (expStack.empty()) {
            LOG_ERROR("[GetCalculatorExpforTernary] fail: middle expression stack is empty.");
            throw nullptr;
        }
        middleExp = expStack.top();
        if (middleExp == nullptr) {
            LOG_ERROR("[GetCalculatorExpforTernary] fail: get middle expression failed.");
            throw nullptr;
        }
        expStack.pop();

        // get left expression
        if (expStack.empty()) {
            LOG_ERROR("[GetCalculatorExpforTernary] fail: left expression stack is empty.");
            throw nullptr;
        }
        leftExp = expStack.top();
        if (leftExp == nullptr) {
            LOG_ERROR("[GetCalculatorExpforTernary] fail: get left expression failed.");
            throw nullptr;
        }
        expStack.pop();

        switch (operatorInfo.operatorID) {
            case IS_JUMP_TYPE_ID: {
                return new IsJumpTypeExp(leftExp, middleExp, lastExp);
            }
            default: {
                throw nullptr;
            }
        }
    } catch (...) {
        LOG_ERROR("[GetCalculatorExpforTernary] fail: catch exception.");
        if (middleExp != nullptr) {
            delete middleExp;
        }

        if (leftExp != nullptr) {
            delete leftExp;
        }
        return nullptr;
    }
}

int32_t EcologicalRuleCalculatorImpl::QueryMatchSceneExperience(const OHOS::AAFwk::Want &want,
    const CallerInfo &callerInfo, const std::string experienceType, ExperienceRule &experienceRule)
{
    experienceRule.isAllow = true;
    int32_t queryRet = NO_MATCH_RULE;
    try {
        LOG_INFO("QueryMatchSceneExperience called");
        // preset instruction execution result cache
        std::map<std::string, CalculatorResult> presetCache;

        std::string transExperienceType = TransRealExperienceType(want, callerInfo, experienceType, presetCache);
        // get data lock must release
        GetRuleStrategyObjectLock();
        EcologicalRuleEngineCache::GetInstance().GetAppEnhanceDataLock();

        int32_t ret =
            EcologicalRuleEngineCache::GetInstance().ParseAppEnhanceItem(callerInfo.packageName, want.GetBundle());
        if (ret != SUCCESS_RULE) {
            LOG_ERROR("[QueryMatchSceneExperience] parse app enhance failed.");
        }
        queryRet = GetSceneExperienceFromCache(want, callerInfo, transExperienceType, presetCache, experienceRule);
    } catch (...) {
        queryRet = NO_MATCH_RULE;
        LOG_ERROR("[QueryMatchSceneExperience] fail: catch exception.");
    }
    // release data lock
    EcologicalRuleEngineCache::GetInstance().ReleaseAppEnhanceDataLock();
    ReleaseRuleStrategyObjectLock();

    if (queryRet != SUCCESS_RULE) {
        experienceRule.isAllow = true;
        experienceRule.sceneCode = "00000";
        LOG_INFO("[QueryMatchSceneExperience] end: not match any rule.");
    }
    LOG_INFO("QueryMatchSceneExperience finish! result is %{public}d", experienceRule.isAllow);
    return SUCCESS_RULE;
}

int32_t EcologicalRuleCalculatorImpl::GetSceneExperienceFromCache(const OHOS::AAFwk::Want &want,
    const CallerInfo &callerInfo, const std::string transExperienceType,
    std::map<std::string, CalculatorResult> &presetCache, ExperienceRule &experienceRule)
{
    int32_t queryRet = NO_MATCH_RULE;
    int32_t ruleCount = 0;
    int32_t rule = 0;
    bool bExempt = false;
    EcologicalRuleEngineCache::GetInstance().GetExemptExperience(want.GetBundle(), rule, bExempt);

    for (auto iter = ruleStrategyObjects.begin(); iter != ruleStrategyObjects.end(); iter++) {
        ruleCount++;
        bool isMatch = CalculateDslMatchResult(want, callerInfo, presetCache, (*iter).dslExp);
        if (!isMatch) {
            continue;
        }
        LOG_INFO("match success!, sceneCode = %{public}s ruleCount = %{public}d", (*iter).sceneCode.c_str(), ruleCount);
        experienceRule.sceneCode = (*iter).sceneCode;
        if (bExempt) {
            LOG_INFO("[GetSceneExperienceFromCache] ExemptExperience exist");
            queryRet = SUCCESS_RULE;
            break;
        }
        nlohmann::json experienceJson = nlohmann::json::parse((*iter).experience, nullptr, false);
        if (experienceJson.is_discarded()) {
            LOG_ERROR("parse experience catch exception!, sceneCode = %{public}s", (*iter).sceneCode.c_str());
            continue;
        }

        int32_t ret = GetExperienceResult(experienceJson, transExperienceType, experienceRule);
        if (ret != SUCCESS_RULE) {
            LOG_ERROR("get experience failed!, sceneCode = %{public}s", (*iter).sceneCode.c_str());
            continue;
        }
        // set scene value cache
        std::string bundleName = GetSceneCodeBundleName(want, experienceRule.sceneCode);
        LOG_INFO("GetSceneExperienceFromCache set SceneCode! bundleName is = %{public}s", bundleName.c_str());
        EcologicalRuleEngineCache::GetInstance().SetSceneCode(bundleName, experienceRule.sceneCode);
        queryRet = SUCCESS_RULE;
        break;
    }

    return queryRet;
}


std::string EcologicalRuleCalculatorImpl::GetSceneCodeBundleName(const OHOS::AAFwk::Want &want,
    const std::string sceneCode)
{
    return want.GetBundle();
}


int32_t EcologicalRuleCalculatorImpl::GetExperienceResult(nlohmann::json &experienceConfig,
    const std::string experienceType, ExperienceRule &experienceRule)
{
    try {
        nlohmann::json *experienceJson;
        nlohmann::json *itConfig = &experienceConfig;
        int32_t ret = EcologicalRuleEngineCache::GetInstance().GetMapItem(itConfig, experienceType, experienceJson);
        if (ret != SUCCESS_RULE) {
            LOG_ERROR("[GetExperienceResult] fail: get experience json object failed.");
            return ERROR_RULE;
        }
        auto experienceValue = experienceJson->get<int>();
        ExperienceResult experienceResult;
        auto it = EXPERIENCE_TYPE_MAP.find(experienceType);
        if (it == EXPERIENCE_TYPE_MAP.end()) {
            LOG_ERROR("[GetExperienceResult] fail: experienceType invalid.");
            return ERROR_RULE;
        }
        ret =
            EcologicalRuleEngineCache::GetInstance().GetSceneExperience(it->second, experienceValue, experienceResult);
        if (ret != SUCCESS_RULE) {
            LOG_ERROR("[GetExperienceResult] fail: get scene experience info failed.");
            return ERROR_RULE;
        }
        experienceRule.isAllow = experienceResult.allow;
        experienceRule.replaceWant = experienceResult.replaceWant;
    } catch (...) {
        LOG_ERROR("[GetExperienceResult] fail: catch exception.");
        return ERROR_RULE;
    }
    return SUCCESS_RULE;
}

std::string EcologicalRuleCalculatorImpl::TransRealExperienceType(const OHOS::AAFwk::Want &want,
    const CallerInfo &callerInfo, const std::string experienceType,
    std::map<std::string, CalculatorResult> &presetCache)
{
    std::string transExperienceType = experienceType;
    CalculatorResult result;
    result.type = BOOL_TYPE;
    result.logicRet = false;
    std::string typeKey = OPERATION_IS_JUMP_TYPE + KEYWORD_CALLER + KEYWORD_WANT;
    try {
        std::string jumpType;
        bool out = false;
        bool in = false;
        bool apps = false;
        if (experienceType.compare(EXPERIENCE_TYPE_LOAD) != 0) {
            jumpType = IsJumpTypeExp::GetJumpType(want, callerInfo);
            if (jumpType.empty()) {
                return transExperienceType;
            }

            if (jumpType.compare(JUMP_TYPE_OUT) == 0) {
                transExperienceType = EXPERIENCE_TYPE_OUT;
                out = true;
            } else if (jumpType.compare(JUMP_TYPE_IN) == 0) {
                transExperienceType = EXPERIENCE_TYPE_OPEN;
                in = true;
            } else {
                transExperienceType = EXPERIENCE_TYPE_OUT;
                apps = true;
            }
        } else {
            jumpType = JUMP_TYPE_IN;
            in = true;
        }

        // set jump type 1 to cache
        result.logicRet = out;
        presetCache.insert(std::pair<std::string, CalculatorResult>(typeKey + JUMP_TYPE_OUT, result));

        // set jump type 2 to cache
        result.logicRet = in;
        presetCache.insert(std::pair<std::string, CalculatorResult>(typeKey + JUMP_TYPE_IN, result));

        // set jump type 3 to cache
        result.logicRet = apps;
        presetCache.insert(std::pair<std::string, CalculatorResult>(typeKey + JUMP_TYPE_APPS, result));
    } catch (...) {
        LOG_ERROR("[TransRealExperienceType] fail: catch exception.");
        return transExperienceType;
    }
    LOG_INFO("finish! get transExperienceType is %{public}s", transExperienceType.c_str());
    return transExperienceType;
}

int32_t EcologicalRuleCalculatorImpl::ConstructRuleStrategyObjects()
{
    GetRuleStrategyObjectLock();
    try {
        ClearRuleStrategyObjects();

        // get rule config root json
        nlohmann::json *rootJson;
        int32_t ret = EcologicalRuleEngineCache::GetInstance().GetEngineDataRoot(KEYWORD_RULES, rootJson);
        if (ret != SUCCESS_RULE) {
            throw nullptr;
        }

        // get rule config list json
        nlohmann::json *listJson;
        ret = EcologicalRuleEngineCache::GetInstance().GetMapItem(rootJson, KEYWORD_LIST, listJson);
        if (ret != SUCCESS_RULE) {
            throw nullptr;
        }

        for (auto it = listJson->begin(); it != listJson->end(); it++) {
            nlohmann::json *arrayItem = &(*it);
            auto itemScene = arrayItem->find(KEYWORD_SCENECODE);
            if (itemScene == arrayItem->end()) {
                continue;
            }
            RuleStrategyObject ruleStrategyObject;
            ruleStrategyObject.sceneCode = itemScene.value();

            // get experience config from json
            auto itemExperience = arrayItem->find(KEYWORD_EXPERIENCE);
            if (itemExperience == arrayItem->end()) {
                continue;
            }

            try {
                ruleStrategyObject.experience = itemExperience->dump();
            } catch (...) {
                continue;
            }

            auto itemCondition = arrayItem->find(KEYWORD_CONDITION);
            if (itemCondition == arrayItem->end()) {
                continue;
            }
            std::string condition = itemCondition.value();
            ruleStrategyObject.dslExp = AnalysisDsl(condition);
            if (ruleStrategyObject.dslExp == nullptr) {
                continue;
            }
            ruleStrategyObjects.push_back(ruleStrategyObject);
        }
    } catch (...) {
        LOG_ERROR("fail: catch exception.");
    }
    LOG_INFO("finish! ruleStrategyObjects size is %{public}lu", ruleStrategyObjects.size());
    ReleaseRuleStrategyObjectLock();
    return SUCCESS_RULE;
}

void EcologicalRuleCalculatorImpl::ClearRuleStrategyObjects()
{
    try {
        for (auto iter = ruleStrategyObjects.begin(); iter != ruleStrategyObjects.end();) {
            delete (*iter).dslExp;
            iter = ruleStrategyObjects.erase(iter);
        }
    } catch (...) {
        LOG_ERROR("[ClearRuleStrategyObjects] fail: catch exception.");
    }
}

void EcologicalRuleCalculatorImpl::GetRuleStrategyObjectLock()
{
    ruleStrategyObjectLock_.lock();
}

void EcologicalRuleCalculatorImpl::ReleaseRuleStrategyObjectLock()
{
    ruleStrategyObjectLock_.unlock();
}

bool EcologicalRuleCalculatorImpl::CalculateDslMatchResult(const OHOS::AAFwk::Want &want, const CallerInfo &callerInfo,
    std::map<std::string, CalculatorResult> &presetCache, BaseExp *DslExp)
{
    if (DslExp == nullptr) {
        return false;
    }
    CalculatorResult calculatorResult;
    try {
        calculatorResult = DslExp->Interpreter(want, callerInfo, presetCache);
    } catch (...) {
        LOG_ERROR("[CalculateDslMatchResult]catch exception.");
        return false;
    }

    if (calculatorResult.type != BOOL_TYPE) {
        return false;
    } else {
        return calculatorResult.logicRet;
    }
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS
