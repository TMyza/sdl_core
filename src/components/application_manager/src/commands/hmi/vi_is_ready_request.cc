/*
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "application_manager/commands/hmi/vi_is_ready_request.h"
#include "application_manager/message_helper.h"

namespace application_manager {

namespace commands {

VIIsReadyRequest::VIIsReadyRequest(const MessageSharedPtr& message,
                                   ApplicationManager& application_manager)
    : RequestToHMI(message, application_manager)
    , EventObserver(application_manager.event_dispatcher()) {}

VIIsReadyRequest::~VIIsReadyRequest() {}

void VIIsReadyRequest::Run() {
  LOG4CXX_AUTO_TRACE(logger_);
  subscribe_on_event(hmi_apis::FunctionID::VehicleInfo_IsReady,
                     correlation_id());
  SendRequest();
}

void VIIsReadyRequest::on_event(const event_engine::Event& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  const smart_objects::SmartObject& message = event.smart_object();
  switch (event.id()) {
    case hmi_apis::FunctionID::VehicleInfo_IsReady: {
      LOG4CXX_DEBUG(logger_, "VehicleInfo_IsReady event");
      unsubscribe_from_event(hmi_apis::FunctionID::VehicleInfo_IsReady);
      const bool is_available =
          ChangeInterfaceState(application_manager_,
                               message,
                               HmiInterfaces::HMI_INTERFACE_VehicleInfo);

      HMICapabilities& hmi_capabilities =
          application_manager_.hmi_capabilities();
      hmi_capabilities.set_is_ivi_cooperating(is_available);
      application_manager_.GetPolicyHandler().OnVIIsReady();
      if (!CheckAvailabilityHMIInterfaces(
              application_manager_, HmiInterfaces::HMI_INTERFACE_VehicleInfo)) {
        LOG4CXX_INFO(
            logger_,
            "HmiInterfaces::HMI_INTERFACE_VehicleInfo isn't available");
        return;
      }
      SendMessageToHMI();

      break;
    }
    default: {
      LOG4CXX_ERROR(logger_, "Received unknown event" << event.id());
      return;
    }
  }
}

void VIIsReadyRequest::onTimeOut() {
  // Note(dtrunov): According to new requirment APPLINK-27956
  SendMessageToHMI();
}

void VIIsReadyRequest::SendMessageToHMI() {
  utils::SharedPtr<smart_objects::SmartObject> get_type(
      MessageHelper::CreateModuleInfoSO(
          hmi_apis::FunctionID::VehicleInfo_GetVehicleType,
          application_manager_));
  application_manager_.ManageHMICommand(get_type);
}

}  // namespace commands

}  // namespace application_manager
