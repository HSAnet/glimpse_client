#include "response.h"
#include "client.h"
#include "settings.h"

#include "timing/timingfactory.h"
#include "channel.h"
#include "log/logger.h"
#include "types.h"

LOGGER(Response);

Response::Response(QObject *parent)
: QObject(parent)
{
}

Response::~Response()
{
}

void Response::finished()
{
}
