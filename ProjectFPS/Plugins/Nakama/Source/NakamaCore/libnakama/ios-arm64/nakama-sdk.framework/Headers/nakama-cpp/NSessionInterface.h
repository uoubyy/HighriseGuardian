/*
 * Copyright 2022 The Nakama Authors
 *
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

#pragma once

#include <string>
#include <memory>

#include <nakama-cpp/NTypes.h>
#include <nakama-cpp/NExport.h>

NAKAMA_NAMESPACE_BEGIN

    class NAKAMA_API NSessionInterface
    {
    public:
        virtual ~NSessionInterface() {}

        /**
         * @return The authentication token used to construct this session.
         */
        virtual const std::string& getAuthToken() const = 0;

        /**
         * @return The refresh token used to construct this session.
         */
        virtual const std::string& getRefreshToken() const = 0;

        /**
         * @return <c>True</c> if the user account for this session was just created.
         */
        virtual bool isCreated() const = 0;

        /**
         * @return The username of the user who owns this session.
         */
        virtual const std::string& getUsername() const = 0;

        /**
         * @return The ID of the user who owns this session.
         */
        virtual const std::string& getUserId() const = 0;

        /**
         * @return The timestamp in milliseconds when this session object was created.
         */
        virtual NTimestamp getCreateTime() const = 0;

        /**
         * @return The timestamp in milliseconds when this session will expire.
         */
        virtual NTimestamp getExpireTime() const = 0;

        /**
         * @return <c>True</c> if the session has expired against the current time.
         */
        virtual bool isExpired() const = 0;

        /**
         * Check if the session's token has expired against the input time.
         *
         * @param now The time to compare against the session.
         *        Use getUnixTimestampMs() to get current time.
         * @return <c>true</c> if the session has expired.
         */
        virtual bool isExpired(NTimestamp now) const = 0;

        /**
         * @return <c>True</c> if the session has expired against the current time.
         */
        virtual bool isRefreshExpired() const = 0;

        /**
         * Check if the session's refresh token has expired against the input time.
         *
         * @param now The time to compare against the session.
         *        Use getUnixTimestampMs() to get current time.
         * @return <c>true</c> if the session has expired.
         */
        virtual bool isRefreshExpired(NTimestamp now) const = 0;

        /**
         * Get session variables.
         *
         * @return NStringMap.
         */
        virtual const NStringMap& getVariables() const = 0;

        /**
         * Get session variable value by name.
         *
         * @return variable value.
         */
        virtual std::string getVariable(const std::string& name) const = 0;
    };

    using NSessionPtr = std::shared_ptr<NSessionInterface>;

    /**
     * Restore a session from an authentication token.
     *
     * @param token The authentication token from a <c>NSessionInterface</c>.
     * @return A session restored from the authentication token.
     */
    NAKAMA_API NSessionPtr restoreSession(const std::string& token, const std::string& refreshToken);

NAKAMA_NAMESPACE_END
