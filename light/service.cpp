/*
 * Copyright 2016 The Android Open Source Project
 * Copyright 2018 The MoKee Open Source Project
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

#define LOG_TAG "android.hardware.light@2.0-service.icesky"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>

#include "Light.h"

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::light::V2_0::ILight;
using android::hardware::light::V2_0::implementation::Light;

const static std::string kLcdBacklightPath = "/sys/class/leds/lcd-backlight/brightness";
const static std::string kLcdMaxBacklightPath = "/sys/class/leds/lcd-backlight/max_brightness";
const static std::string kBlueLedPath = "/sys/class/leds/blue/brightness";
const static std::string kBlueDutyPctsPath = "/sys/class/leds/blue/duty_pcts";
const static std::string kBlueStartIdxPath = "/sys/class/leds/blue/start_idx";
const static std::string kBluePauseLoPath = "/sys/class/leds/blue/pause_lo";
const static std::string kBluePauseHiPath = "/sys/class/leds/blue/pause_hi";
const static std::string kBlueRampStepMsPath = "/sys/class/leds/blue/ramp_step_ms";
const static std::string kBlueBlinkPath = "/sys/class/leds/blue/blink";

int main() {
    uint32_t lcdMaxBrightness = 255;

    std::ofstream lcdBacklight(kLcdBacklightPath);
    if (!lcdBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ifstream lcdMaxBacklight(kLcdMaxBacklightPath);
    if (!lcdMaxBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdMaxBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    } else {
        lcdMaxBacklight >> lcdMaxBrightness;
    }

    std::ofstream blueLed(kBlueLedPath);
    if (!blueLed) {
        LOG(ERROR) << "Failed to open " << kBlueLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueDutyPcts(kBlueDutyPctsPath);
    if (!blueDutyPcts) {
        LOG(ERROR) << "Failed to open " << kBlueDutyPctsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueStartIdx(kBlueStartIdxPath);
    if (!blueStartIdx) {
        LOG(ERROR) << "Failed to open " << kBlueStartIdxPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePauseLo(kBluePauseLoPath);
    if (!bluePauseLo) {
        LOG(ERROR) << "Failed to open " << kBluePauseLoPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePauseHi(kBluePauseHiPath);
    if (!bluePauseHi) {
        LOG(ERROR) << "Failed to open " << kBluePauseHiPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueRampStepMs(kBlueRampStepMsPath);
    if (!blueRampStepMs) {
        LOG(ERROR) << "Failed to open " << kBlueRampStepMsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueBlink(kBlueBlinkPath);
    if (!blueBlink) {
        LOG(ERROR) << "Failed to open " << kBlueBlinkPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    android::sp<ILight> service = new Light(
            {std::move(lcdBacklight), lcdMaxBrightness},
            std::move(blueLed),
            std::move(blueDutyPcts),
            std::move(blueStartIdx),
            std::move(bluePauseLo),
            std::move(bluePauseHi),
            std::move(blueRampStepMs),
            std::move(blueBlink));

    configureRpcThreadpool(1, true);

    android::status_t status = service->registerAsService();

    if (status != android::OK) {
        LOG(ERROR) << "Cannot register Light HAL service";
        return 1;
    }

    LOG(INFO) << "Light HAL Ready.";
    joinRpcThreadpool();
    // Under normal cases, execution will not reach this line.
    LOG(ERROR) << "Light HAL failed to join thread pool.";
    return 1;
}
