#pragma once

#include "Core.h"
#pragma warning(push, 0)
#include <memory>
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
//#define SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace tnah
{
	/**
	 * @class	Log
	 *
	 * @brief	A logging class that handles loggers for the core engine and the client
	 *
	 * @author	Bryce Standley
	 * @date	7/09/2021
	 */

	class Log
	{
	public:

		/**
		 * @fn	static void Log::Init();
		 *
		 * @brief	Initializes this object
		 *
		 * @author	Bryce Standley
		 * @date	7/09/2021
		 */

		static void Init();

		/**
		* @fn	static void Log::SetLoggerDebugMode();
		*
		* @brief	Changes the Output of the logger to also display extra debug infomation
		*
		* @author	Bryce Standley
		* @date	7/09/2021
		*/
		static void SetLoggerDebugMode();

		/**
		* @fn	static void Log::SetLoggerCoreMode();
		*
		* @brief	Changes the Output of the logger to also display extra debug infomation
		*
		* @author	Bryce Standley
		* @date	7/09/2021
		*/
		static void SetLoggerCoreMode();

		/**
		 * @fn	inline static std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()
		 *
		 * @brief	Gets core logger
		 *
		 * @author	Bryce Standley
		 * @date	7/09/2021
		 *
		 * @returns	The core logger.
		 */

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

		/**
		 * @fn	inline static std::shared_ptr<spdlog::logger>& Log::GetClientLogger()
		 *
		 * @brief	Gets client logger
		 *
		 * @author	Bryce Standley
		 * @date	7/09/2021
		 *
		 * @returns	The client logger.
		 */

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	
	private:

		/** @brief	The core logger */
		static std::shared_ptr<spdlog::logger> s_CoreLogger;

		/** @brief	The client logger */
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		
	};
}

// Core log macros
#define TNAH_CORE_TRACE(...)    ::tnah::Log::GetCoreLogger()->trace(__VA_ARGS__) // White
#define TNAH_CORE_INFO(...)     ::tnah::Log::GetCoreLogger()->info(__VA_ARGS__)// Green
#define TNAH_CORE_WARN(...)     ::tnah::Log::GetCoreLogger()->warn(__VA_ARGS__) // Yellow
#define TNAH_CORE_ERROR(...)    ::tnah::Log::GetCoreLogger()->error(__VA_ARGS__) // Red
#define TNAH_CORE_FATAL(...)    ::tnah::Log::GetCoreLogger()->fatal(__VA_ARGS__) // White with red highlight

// Client log macros
#define TNAH_TRACE(...)	      ::tnah::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TNAH_INFO(...)	      ::tnah::Log::GetClientLogger()->info(__VA_ARGS__)
#define TNAH_WARN(...)	      ::tnah::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TNAH_ERROR(...)	      ::tnah::Log::GetClientLogger()->error(__VA_ARGS__)
#define TNAH_FATAL(...)	      ::tnah::Log::GetClientLogger()->fatal(__VA_ARGS__)