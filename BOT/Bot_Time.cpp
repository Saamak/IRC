#include "includes.hpp"

std::string get_current_time_in_country(const std::string& country)
{
    int offset_hours = 0;

    if (country == "FRANCE")
        offset_hours = OFFSET_FRANCE;
    else if (country == "PAKISTAN")
        offset_hours = OFFSET_PAKISTAN;
    else if (country == "USA")
        offset_hours = OFFSET_USA;
    else if (country == "BRAZIL")
        offset_hours = OFFSET_BRAZIL;
    else if (country == "UK")
        offset_hours = OFFSET_UK;
    else if (country == "JAPAN")
        offset_hours = OFFSET_JAPAN;
    else if (country == "AUSTRALIA")
        offset_hours = OFFSET_AUSTRALIA;
    else if (country == "RUSSIA")
        offset_hours = OFFSET_RUSSIA;
    else if (country == "CHINA")
        offset_hours = OFFSET_CHINA;
    else
        return ("Unknown Country");

    std::time_t current_time = std::time(NULL);
    std::tm* utc_time = std::gmtime(&current_time);
    utc_time->tm_hour += offset_hours;

    if (utc_time->tm_hour >= 24)
    {
        utc_time->tm_hour -= 24;
        utc_time->tm_mday += 1;
    }
    else if (utc_time->tm_hour < 0)
    {
        utc_time->tm_hour += 24;
        utc_time->tm_mday -= 1;
    }

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", utc_time);
    return std::string(buffer);
}

void BotTime(int sockfd, const std::string country, const std::string channel)
{
	if (country == "LIST")
    {
        std::string join_message = "PRIVMSG " + channel + " :Country List :-FRANCE-PAKISTAN-USA-BRAZIL-UK-JAPAN-AUSTRALIA-RUSSIA-CHINA\r\n";
        const char *str = join_message.c_str();
        send_command(sockfd, str); 
    }
    else
    {
        std::string time = get_current_time_in_country(country);
        if (time == "Unknown Country")
        {
            std::string join_message = "PRIVMSG " + channel + " :Unknown Country, can't display the time\r\n";
            const char *str = join_message.c_str();
            send_command(sockfd, str);
        }
        else
        {
            std::string join_message = "PRIVMSG " + channel + " :In " + country + " it's : " + time + "\r\n";
            const char *str = join_message.c_str();
            send_command(sockfd, str);
        }
    }
}