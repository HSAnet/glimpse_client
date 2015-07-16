#include "resultcontroller.h"
#include "../storage/storagepaths.h"
#include "../settings.h"

#include <QPointer>
#include <QDir>
#include <QDateTime>

class ResultController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ResultController *q)
    : q(q)
    {

    }

    ResultController *q;

    QPointer<ResultScheduler> scheduler;
    QPointer<Settings> settings;

public slots:
    void rotate();
};

void ResultController::Private::rotate()
{
    // <task-id>_yyyy-MM-dd.json
    QRegExp regex("^\\d+_(\\d{4}-\\d{2}-\\d{2}).json$");
    QDir dir(StoragePaths().resultDirectory());
    QDate oldest = QDateTime::currentDateTime().addDays(-static_cast<qint64>(settings->backlog())).date();

    foreach (const QString &file, dir.entryList(QDir::Files))
    {
        if (regex.exactMatch(file))
        {
            if (QDateTime::fromString(regex.cap(1), "yyyy-MM-dd").date() < oldest)
            {
                dir.remove(file);
            }
        }
    }
}

ResultController::ResultController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

ResultController::~ResultController()
{
    d->rotate();
    delete d;
}

bool ResultController::init(ResultScheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;

    connect(d->scheduler, SIGNAL(resultAdded(const QVariantMap)), d, SLOT(rotate()));

    return true;
}

QString ResultController::errorString() const
{
    return "";
}

ResultController::Status ResultController::status() const
{
    return Unknown;
}

#include "resultcontroller.moc"
