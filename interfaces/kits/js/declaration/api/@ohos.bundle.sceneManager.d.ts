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
 * Provides functions for the scene manager.
 *
 * @namespace sceneManager
 * @syscap SystemCapability.BundleManager.EcologicalRuleManager
 * @since 10
 */
declare namespace sceneManager {
  /**
   * Obtains the scene code by bundle name.
   *
   * @permission ohos.permission.GET_SCENE_CODE
   * @param { string } bundleName - Bundle name.
   * @returns { string } Returns the scene code.
   * @throws { BusinessError } 201 - Permission denied.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @since 10
   */
  function getSceneCode(bundleName: string): string;

  /**
   * Obtains the initial scene code by bundle name.
   *
   * @permission ohos.permission.GET_SCENE_CODE
   * @param { string } bundleName - Bundle name.
   * @returns { string } Returns the initial scene code.
   * @throws { BusinessError } 201 - Permission denied.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @since 10
   */
  function getInitialSceneCode(bundleName: string): string;

  /**
   * Obtains the own scene code.
   *
   * @returns { string } Returns the scene code of the caller.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @since 10
   */
  function getSelfSceneCode(): string;

  /**
   * Obtains the ads verification version of the ecological rule manager.
   *
   * @returns { string } Returns the version of ads verification.
   * @syscap SystemCapability.BundleManager.EcologicalRuleManager
   * @since 10
   */
  function getAdsVerificationVersion(): string;
}

export default sceneManager;
