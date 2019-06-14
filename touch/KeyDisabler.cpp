/*
 * Copyright (C) 2019 The MoKee Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <linux/input.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include "KeyDisabler.h"

namespace vendor {
namespace mokee {
namespace touch {
namespace V1_0 {
namespace implementation {

constexpr const char kTouchControlPath[] = "/proc/keypad/home_touch";
constexpr const char kPressControlPath[] = "/proc/keypad/home_press";

KeyDisabler::KeyDisabler() {
    mHasTouchKey = !access(kTouchControlPath, F_OK);
    mHasPressKey = !access(kPressControlPath, F_OK);
}

// Methods from ::vendor::mokee::touch::V1_0::IKeyDisabler follow.
Return<bool> KeyDisabler::isEnabled() {
    std::string buf;

    if (!mHasTouchKey) return false;

    if (!android::base::ReadFileToString(kTouchControlPath, &buf, true)) {
        LOG(ERROR) << "Failed to read " << kTouchControlPath;
        return false;
    }

    return std::stoi(android::base::Trim(buf)) == 0;
}

Return<bool> KeyDisabler::setEnabled(bool enabled) {
    std::string touchValue = std::to_string(enabled ? 0 : KEY_BACK);
    std::string pressValue = std::to_string(enabled ? 0 : KEY_HOMEPAGE);

    if (!mHasTouchKey) return false;

    if (!android::base::WriteStringToFile(touchValue, kTouchControlPath, true)) {
        LOG(ERROR) << "Failed to write " << kTouchControlPath;
        return false;
    }

    if (mHasPressKey) {
        if (!android::base::WriteStringToFile(pressValue, kPressControlPath, true)) {
            LOG(ERROR) << "Failed to write " << kPressControlPath;
            return false;
        }
    }

    return true;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace mokee
}  // namespace vendor
