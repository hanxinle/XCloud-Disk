﻿#include "XUploadTask.h"
#include <iostream>
using namespace std;

// 接收消息回调
bool XUploadTask::ReadCB (const XMsg * msg) {
    switch (msg->type) {
    case MSG_UPLOAD_ACCEPT:
        BeginWrite ();
        break;
    case MSG_UPLOAD_COMLETE:
        // 界面刷新
        if (UploadCB)
            UploadCB ();
        break;
    default:
        break;
    }
    return true;
}
// 连接成功回调
void XUploadTask::ConnectedCB () {
    if (filepath_.empty ()) {
        cerr << " XUploadTask::ConnectedCB filepath_ is empty" << endl;
        return;
    }
    // 1 打开本地文件
    ifs_.open (filepath_.c_str (), ios::in | ios::binary| ios::ate);
    if (!ifs_.is_open ()) {
        cerr << "open file " << filepath_ << " failed" << endl;
        return;
    }
    // 2 获取文件名称，大小 filename.zip,2334 
    filesize_ = ifs_.tellg ();
    ifs_.seekg (0, ios::beg);
    cout << "open file " << filepath_ << " success." << endl;
    string filename = "";
    int pos = filepath_.find_last_of ('/');
    if (pos < 0)
        pos = 0;
    filename = filepath_.substr (pos, filepath_.size ()-pos);
    cout << "Gui filename = " << filename << endl;
    char data[1024] = {0};
    sprintf (data, "%s,%d", filename.c_str (), filesize_);
    cout << "GUI send data = " << data << endl;
    // 3 发送上传文件请求
    XMsg msg;
    msg.type = MSG_UPLOAD_INFO;
    msg.data = data;
    msg.size = strlen (data)+1;
    Write (&msg);
}

void XUploadTask::WriteCB () {
    if (!ifs_.is_open ())
        return;
    // 读取内容到 read_buf_
    ifs_.read (read_buf_, sizeof (read_buf_));
    int len = ifs_.gcount ();
    if (len <= 0) {
        ifs_.close ();
        return;
    }
    Write (read_buf_, len);
    if (ifs_.eof ()) {
        ifs_.close ();
    }
}
