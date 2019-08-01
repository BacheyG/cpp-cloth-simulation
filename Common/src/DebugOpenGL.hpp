#ifndef __DEBUGOPENGL__
#define __DEBUGOPENGL__

#ifdef _WIN32
#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp")

#define STDCALL __stdcall
#else
#define STDCALL
#endif

#ifdef _LINUX
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cxxabi.h>

extern const char *__progname;

#endif

#include<iostream>
#include<ostream>
#include<string>

#include <GL/glew.h>

class DebugOpenGL {

public:

	/** Inits the library, sets the Callback function
	*
	* \param outStream where to record the message log
	* NULL means cout
	*
	*/
	static void init(std::ostream *outStream = NULL);

	/** Adds an application event to the log
	*
	* \param id the event id
	* \param type event's type. Can be one of the following:
	GL_DEBUG_TYPE_ERROR_ARB
	GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
	GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
	GL_DEBUG_TYPE_PORTABILITY_ARB
	GL_DEBUG_TYPE_PERFORMANCE_ARB
	GL_DEBUG_TYPE_OTHER_ARB
	* \param severity event's severity. Can be one of the following:
	GL_DEBUG_SEVERITY_HIGH_ARB
	GL_DEBUG_SEVERITY_MEDIUM_ARB
	GL_DEBUG_SEVERITY_LOW_ARB
	* \param message  event's message
	*/
	static void addApplicationMessage(GLuint id, GLenum type,
		GLenum severity, std::string message);

	/** Adds a third party event to the log
	*
	* \param id the event id
	* \param type event's type. Can be one of the following:
	GL_DEBUG_TYPE_ERROR_ARB
	GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
	GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
	GL_DEBUG_TYPE_PORTABILITY_ARB
	GL_DEBUG_TYPE_PERFORMANCE_ARB
	GL_DEBUG_TYPE_OTHER_ARB
	* \param severity event's severity. Can be one of the following:
	GL_DEBUG_SEVERITY_HIGH_ARB
	GL_DEBUG_SEVERITY_MEDIUM_ARB
	GL_DEBUG_SEVERITY_LOW_ARB
	* \param message  event's message
	*/
	static void addThirdPartyMessage(GLuint id, GLenum type,
		GLenum severity, std::string message);

	/// Enable/Disable the callback
	static void enableCallback(bool enable);

	/// Enalbe/Disable Low severity events
	static void enableLowSeverityMessages(bool enabled);

	/// Enables/Disables app and third party events
	static void enableUserMessages(bool enabled);

	/// Sets user param for the callback
	static void setUserParam(void *p);

	/// clears the message log
	static void clearMessageLog();


private:
	DebugOpenGL() {};
	~DebugOpenGL(){};

	/// The stream to where messages are directed
	static std::ostream *spOuts;
	/// Pointer to user data
	static void *spUserParam;

	/// Helper functions to print human-readable message
	static std::string getStringForSource(GLenum source);
	static std::string getStringForType(GLenum type);
	static std::string getStringForSeverity(GLenum type);

	/// The function to print the call stack
#if (defined _WIN32 || defined _LINUX)
	static void printStack();
#endif
#ifdef _LINUX
	static int getFileAndLine(unw_word_t addr, char *file, size_t flen, int *line);
#endif

	/// our callback
	static void CALLBACK DebugLog(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);

};
#endif