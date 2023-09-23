#include <iostream>
#include "base/conf.h"
#include "base/log.h"

xrtc::Generalconf *g_conf = nullptr;
xrtc::XrtcLog *g_log = nullptr;

int init_general_conf(const char *filename)
{
    if (!filename)
    {
        fprintf(stderr, "filename is nullptr");
        return -1;
    }

    g_conf = new xrtc::Generalconf();

    int ret = load_general_conf(filename, g_conf);
    if (ret != 0)
    {
        fprintf(stderr, "load %s config file failed\n", filename);
        return -1;
    }

    return 0;
}

int init_log(const std::string &log_dir, const std::string &log_name,
             const std::string &log_level)
{
    g_log = new xrtc::XrtcLog(log_dir, log_name, log_level);
    int ret = g_log->init();
    if (ret != 0)
    {
        fprintf(stderr, "init log failed\n");
        return -1;
    }

    ret = g_log->start();
    if (!ret)
    {
        fprintf(stderr, "log thread start failed\n");
        return -1;
    }

    return 0;
}

int main()
{
    int ret = init_general_conf("./conf/general.yaml");
    if (ret != 0)
    {
        return -1;
    }

    ret = init_log(g_conf->log_dir, g_conf->log_name, g_conf->log_level);
    if (ret != 0)
    {
        return -1;
    }

    g_log->set_log_to_stderr(g_conf->log_to_stderr);

    RTC_LOG(LS_VERBOSE) << "hello world";
    RTC_LOG(LS_WARNING) << "hello world LS_WARNING";
    RTC_LOG(LS_ERROR) << "hello world LS_ERROR";
    g_log->join();
    return 0;
}