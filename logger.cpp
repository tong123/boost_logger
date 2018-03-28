#include "logger.h"
#include <boost/log/common.hpp>  
#include <boost/log/expressions.hpp>  
#include <boost/log/utility/setup/file.hpp>  
#include <boost/log/utility/setup/console.hpp>  
#include <boost/log/utility/setup/common_attributes.hpp>  
#include <boost/log/attributes/timer.hpp>  
#include <boost/log/attributes/named_scope.hpp>  
#include <boost/log/sources/logger.hpp>  
#include <boost/log/support/date_time.hpp>  
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

typedef boost::unique_lock<boost::shared_mutex> write_lock;
using boost::shared_ptr;
static src::severity_logger< Logger::Severity_Level > slg;
static boost::shared_mutex mutex;

template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
	std::basic_ostream< CharT, TraitsT >& strm, Logger::Severity_Level lvl)
{
	static const char* const str[] =
	{
		"DEBUG",
		"INFO",
		"EVENT",
		"ALARM",
		"ERROR",
	};
	if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
		strm << str[lvl];
	else
		strm << static_cast<int>(lvl);
	return strm;
}

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::initial()
{
	//logging::add_console_log(std::clog, keywords::format = "%TimeStamp%: %Message%");

	logging::add_file_log
	(
		"sample.log",
		keywords::open_mode= std::ios::app,
		keywords::rotation_size = 10*1024*1024,
		keywords::min_free_space = 20*1024*1024,
		keywords::filter = expr::attr< Severity_Level >("Severity") >= DEBUG,
		keywords::format = expr::stream
		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "<%Y-%m-%d %H:%M:%S>")
		<< "[" << expr::attr< Severity_Level >("Severity")
		<< "] " << expr::message
	);
	logging::add_common_attributes();
	logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());
	slg.add_attribute("Uptime", attrs::timer());
}
void Logger::log_write(Severity_Level level, std::string func_name, std::string os)
{
	write_lock(mutex);
	BOOST_LOG_SEV(slg, level) << "["<<func_name<<"] "<<os;
}
#define LOG_W(level,param) Logger::log_write(level, __FUNCTION__, param);

int main(int argc, char* argv[])
{
	Logger::initial();
	LOG_W(Logger::Severity_Level::DEBUG, std::string("hello3")+ std::to_string(1));
	return 0;
} 
