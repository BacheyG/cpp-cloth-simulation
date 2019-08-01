#include "DebugOpenGL.hpp"
#include <string.h>

#ifdef _LINUX
#include <execinfo.h>
#endif


std::ostream *DebugOpenGL::spOuts = NULL;
void *DebugOpenGL::spUserParam = NULL;

// Init the library with a stream to where the output
// will be directed. NULL means cout
void
DebugOpenGL::init(std::ostream *outStream) {

	// set the output stream
	if (!outStream)
		spOuts = (std::iostream *)&std::cout;
	else
		spOuts = outStream;

	// check if the extension is there
	char *s;
	int i = 0, max;
	glGetIntegerv(GL_NUM_EXTENSIONS, &max);
	do {
		s = (char *)glGetStringi(GL_EXTENSIONS, i++);
	} while (i < max && strcmp(s, "GL_ARB_debug_output") != 0);

	// if we have the extension then ...
	if (s != NULL) {
		// enable sync mode and set the callback
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(DebugLog, spUserParam);
	}
	else
		// extension has not been loaded
		// report it back
		*spOuts << "VSDebugLib Error: OpenGL Debug Context not enabled\n";


}

// Sets the pointer to user data
void
DebugOpenGL::setUserParam(void *p) {

	spUserParam = p;
	glDebugMessageCallbackARB(DebugLog, DebugOpenGL::spUserParam);
}

// clears the message log
void
DebugOpenGL::clearMessageLog() {

	int totalMessages;
	glGetIntegerv(GL_DEBUG_LOGGED_MESSAGES_ARB, &totalMessages);
	glGetDebugMessageLogARB(totalMessages, 0, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Adds an application event
void
DebugOpenGL::addApplicationMessage(GLuint id, GLenum type, GLenum severity, std::string message) {

	glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB, type, id,
		severity, -1, message.c_str());
}

// Adds a third party event
void
DebugOpenGL::addThirdPartyMessage(GLuint id, GLenum type, GLenum severity, std::string message) {

	glDebugMessageInsertARB(GL_DEBUG_SOURCE_THIRD_PARTY_ARB, type, id,
		severity, -1, message.c_str());
}

// Enables/disables the callback
void
DebugOpenGL::enableCallback(bool enable) {

	if (enable)
		glDebugMessageCallbackARB(DebugLog, NULL);
	else
		glDebugMessageCallbackARB(NULL, NULL);
}

// Enables/disables low severity events
void
DebugOpenGL::enableLowSeverityMessages(bool enabled) {

	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE,
		GL_DEBUG_SEVERITY_LOW_ARB, 0, NULL, enabled);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE,
		GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, enabled);
}

// enables/disables user events (app and third party)
void
DebugOpenGL::enableUserMessages(bool enabled) {

	glDebugMessageControlARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DONT_CARE,
		GL_DONT_CARE, 0, NULL, enabled);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_THIRD_PARTY_ARB, GL_DONT_CARE,
		GL_DONT_CARE, 0, NULL, enabled);
}

// --------------------------------------------------



void /*CALLBACK */
DebugOpenGL::DebugLog(GLenum source,
GLenum type,
GLuint id,
GLenum severity,
GLsizei length,
const GLchar* message,
const void* userParam) {

	*spOuts << " -- \n" << "Type: " <<
		getStringForType(type).c_str() << "; Source: " <<
		getStringForSource(source).c_str() << "; ID: " << id << "; Severity: " <<
		getStringForSeverity(severity).c_str() << "\n" << message << "\n";

#if (defined _WIN32 || defined _LINUX)
	printStack();
#endif

	//	exit(1);
}

// aux function to translate source to string
std::string
DebugOpenGL::getStringForSource(GLenum source) {

	switch (source) {
	case GL_DEBUG_SOURCE_API_ARB:
		return("API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		return("Window System");
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		return("Shader Compiler");
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		return("Third Party");
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		return("Application");
	case GL_DEBUG_SOURCE_OTHER_ARB:
		return("Other");
	default:
		return("");
	}
}

// aux function to translate severity to string
std::string
DebugOpenGL::getStringForSeverity(GLenum severity) {

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		return("High");
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		return("Medium");
	case GL_DEBUG_SEVERITY_LOW_ARB:
		return("Low");
	default:
		return("");
	}
}

// aux function to translate type to string
std::string
DebugOpenGL::getStringForType(GLenum type) {

	switch (type) {
	case GL_DEBUG_TYPE_ERROR_ARB:
		return("Error");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		return("Deprecated Behaviour");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		return("Undefined Behaviour");
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		return("Portability Issue");
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		return("Performance Issue");
	case GL_DEBUG_TYPE_OTHER_ARB:
		return("Other");
	default:
		return("");
	}
}

// output the call stack
#ifdef _WIN32
void DebugOpenGL::printStack() {

	unsigned int   i;
	void         * stack[100];
	unsigned short frames;
	SYMBOL_INFO  * symbol;
	HANDLE         process;

	process = GetCurrentProcess();

	SymSetOptions(SYMOPT_LOAD_LINES);

	SymInitialize(process, NULL, TRUE);

	frames = CaptureStackBackTrace(0, 200, stack, NULL);
	symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (i = 0; i < frames; i++)
	{
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
		DWORD  dwDisplacement;
		IMAGEHLP_LINE64 line;

		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		if (!strstr(symbol->Name, "VSDebugLib::") &&
			SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dwDisplacement, &line)) {

			*spOuts << "function: " << symbol->Name <<
				" - line: " << line.LineNumber << "\n";

		}
		if (0 == strcmp(symbol->Name, "main"))
			break;
	}

	free(symbol);
}
#endif

#ifdef _LINUX
int DebugOpenGL::getFileAndLine(unw_word_t addr, char *file, size_t flen, int *line)
{
	static char buf[256];
	char *p;

	// prepare command to be executed
	// our program need to be passed after the -e parameter
	sprintf(buf, "/usr/bin/addr2line -C -e ./%s -f -i %lx", __progname, addr);

	FILE* f = popen(buf, "r");

	if (f == NULL)
	{
		perror(buf);
		return 0;
	}

	// get function name
	fgets(buf, 256, f);

	// get file and line
	fgets(buf, 256, f);

	if (buf[0] != '?')
	{
		int l;
		char *p = buf;

		// file name is until ':'
		while (*p != ':')
		{
			p++;
		}

		*p++ = 0;
		// after file name follows line number
		strcpy(file, buf);
		sscanf(p, "%d", line);
	}
	else
	{
		strcpy(file, "unknown");
		*line = -1;
	}

	pclose(f);
}

void DebugOpenGL::printStack() {

	unw_cursor_t cursor;
	unw_context_t uc;
	unw_word_t ip, sp, off;
	unw_proc_info_t pi;
	char file[256], name[256];
	int line;
	int status;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	while (unw_step(&cursor) > 0) {
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);

		unw_get_proc_name(&cursor, name, sizeof(name), &off);
		getFileAndLine((long)ip, file, 256, &line);

		if (line >= 0) {
			char *realname;
			realname = abi::__cxa_demangle(name, 0, 0, &status);

			if (realname) {
				printf("%s: %s, %d\n", realname, file, line);
				free(realname);
			}
			else {
				printf("%s: %s, %d\n", name, file, line);
			}
		}
	}
}
#endif
