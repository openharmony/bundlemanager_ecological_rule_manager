/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

/**
 * Provides functions for the ecological rule manager.
 *
 * @namespace ecologicalRuleDataManager
 * @syscap SystemCapability.BundleManager.EcologicalRuleManager
 * @systemapi
 * @since 10
 */
declare namespace ecologicalRuleDataManager {
  /**
   * Sets a rule into the ecological rule manager.
   *
   * @permission ohos.permission.MANAGE_ECOLOGICAL_RULE
   * @param { string } ruleInfo - Rule information.
   * @returns { number } Returns the result code of this operation.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Permission denied. A non-system application calls a system API.
   * @throws { BusinessError } 401 - The parameter check failed.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @systemapi
   * @since 10
   */
  function setRuleInfo(ruleInfo: string): number;

  /**
   * Sets scene experience corresponding to a rule into the ecological rule manager.
   *
   * @permission ohos.permission.MANAGE_ECOLOGICAL_RULE
   * @param { string } ruleConfig - Rule information.
   * @param { string } sceneExperience - Scene experience.
   * @returns { number } Returns the result code of this operation.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Permission denied. A non-system application calls a system API.
   * @throws { BusinessError } 401 - The parameter check failed.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @systemapi
   * @since 10
   */
  function setSceneExperience(ruleConfig: string, sceneExperience: string): number;

  /**
   * Exempts experience for a bundle, which will be allowed by the ecological rule manager with a given duration.
   *
   * @permission ohos.permission.MANAGE_ECOLOGICAL_RULE
   * @param { string } targetBundleName - Name of the target bundle.
   * @param { number } rule - Rule to be exempted.
   * @param { number } timestamp - Duration for which the exemption is valid.
   * @returns { number } Returns the result code of this operation.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Permission denied. A non-system application calls a system API.
   * @throws { BusinessError } 401 - The parameter check failed.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @systemapi
   * @since 10
   */
  function exemptExperience(targetBundleName: string, rule: number, timestamp: number): number;

  /**
   * Obtains the version of the ecological rule manager.
   *
   * @permission ohos.permission.MANAGE_ECOLOGICAL_RULE
   * @returns { string } Returns the version.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Permission denied. A non-system application calls a system API.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @systemapi
   * @since 10
   */
  function getVersion(): string;

  /**
   * Sets enhanced application data into the ecological rule manager.
   *
   * @permission ohos.permission.MANAGE_ECOLOGICAL_RULE
   * @param { number } operationType - Operation type, which can be -1 (delete data), 0 (full replace), or 1 (append data).
   * @param { string } appData - Enhanced application data.
   * @returns { number } Returns the result code of this operation.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Permission denied. A non-system application calls a system API.
   * @throws { BusinessError } 401 - The parameter check failed.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @systemapi
   * @since 10
   */
  function setAppEnhancedData(operationType: number, appData: string): number;
}

export default ecologicalRuleDataManager;
