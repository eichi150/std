#ifndef ENUM_CLASS_H
#define ENUM_CLASS_H

enum class error{
	  ok = 0
	, unknown
	, not_found
	, synthax
	, untitled_error
	, double_entity
	, double_alias
	, unknown_alias
	, unknown_alias_or_entity
	, alias_equal_entity
	, user_input_is_command
	, unknown_language
	, sensor
};
enum class command{
	  help = 0
	, add
	, delete_
	, show
	, hours
	, minutes 
	, config_filepath
	, user_filepath
	, language
	, sensor
	, save_sensor_data
};

enum class Language{
	  english = 0
	, german 
};

#endif //ENUM_CLASS_H
