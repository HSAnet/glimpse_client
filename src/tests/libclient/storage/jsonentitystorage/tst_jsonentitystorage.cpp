#include <QtTest>
#include <QVariantMap>
#include <QDebug>

#include <storage/jsonentitystorage.h>
#include <report/reportstorage.h>

class tst_Storage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void tst_bson();
    void tst_idType();
    void tst_resultSharedData();
    void tst_resultDataTypes();
    void tst_entity();
    void tst_storage();
};

void tst_Storage::initTestCase()
{
}

void tst_Storage::cleanupTestCase()
{
}

void tst_Storage::tst_bson()
{
    QVariantMap map;
    map.insert("name", "paul");
    map.insert("age", 12);
    map.insert("list", QVariantList() << 1337 << 12 << 8);

    bson b;
    bson_init(&b);
    bsonFromVariant(&b, map);
    bson_finish(&b);

    QVariant variant = variantFromBson(&b);
    QCOMPARE(variant.type(), QVariant::Map);
    QCOMPARE(variant.toMap(), map);
}

void tst_Storage::tst_idType()
{
    /*
    Ident<Result> resultId(1);
    Ident<Report> reportId(1);
    */

    // This must not compile
    //QVERIFY(resultId != reportId);
}

void tst_Storage::tst_resultSharedData()
{
    /*
    Result a;
    a.setAge(5);

    Result b = a;
    QCOMPARE(b.age(), a.age());

    b.setAge(10);
    QCOMPARE(a.age(), 5);#
    */
}

void tst_Storage::tst_resultDataTypes()
{
    /*
    Result a;

    QVariant type = a.toVariant();
    QCOMPARE(type.type(), QVariant::Map);

    QVariantMap map = type.toMap();
    QVariant age = map.value("age");
    QVariant name = map.value("name");

    QCOMPARE(age.type(), QVariant::Int);
    QCOMPARE(name.type(), QVariant::String);
    */
}

void tst_Storage::tst_entity()
{
    /*
    QMap<QString, QVariant::Type> types;

    Entity<Result> resultEntity;
    types = resultEntity.parameters();
    QCOMPARE(types.value("name"), QVariant::String);
    QCOMPARE(types.value("age"), QVariant::Int);

    Entity<Report> reportEntity;
    types = reportEntity.parameters();
    QCOMPARE(types.value("id"), QVariant::UserType);
    QCOMPARE(types.value("results"), QVariant::List);

    QMap<QString, int> listTypes = reportEntity.listTypes();
    QCOMPARE(listTypes.value("results"), qMetaTypeId<ResultId>());
    */
}

void tst_Storage::tst_storage()
{
    ReportStorage storage(NULL);

    // Initialize the storage
    QCOMPARE(storage.init("result"), true);

    /*
    ////////////////////////////////////////////////////////
    // RESULTS
    // Add two datasets
    {
        Result result;
        result.setName("testhans");
        result.setAge(4);

        QCOMPARE(storage.add(result), ResultId(1));

        result.setName("paul");
        QCOMPARE(storage.add(result), ResultId(2));
    }

    // Read two datasets and modify one
    {
        Result result = storage->entityById(1);
        QCOMPARE(result.name(), QString("testhans"));
        QCOMPARE(result.age(), 4);

        result = storage->entityById(2);
        QCOMPARE(result.name(), QString("paul"));

        // Modify dataset
        result.setName("dieter");
        storage->update(result);
        result = storage->entityById(2);
        QCOMPARE(result.name(), QString("dieter"));
    }

    // Delete a dataset
    {
        QCOMPARE(storage->remove(ResultId(1)), true);

        // Make sure it's gone
        QCOMPARE(storage->entityById(ResultId(1)).id(), ResultId());
    }

    // Fetch the remaining dataset
    {
        QList<Result> results = storage->allEntities();
        QCOMPARE(results.size(), 1);
    }
    */

    ////////////////////////////////////////////////////////
    // Reports

    QList<Report> allReports = storage.allEntities();

    Report report;
    report.setResults(QList<Result>() << Result());

    QCOMPARE(storage.add(QList<Report>() << report).first().isValid(), true);

    QList<Report> allReports2 = storage.allEntities();
    QCOMPARE(allReports2.size(), allReports.size()+1);



    //Report newReport = storage.entityById(ReportId(1));
    //QCOMPARE(newReport.id(), ReportId(1));
    //QCOMPARE(newReport.results(), report.results());
}

QTEST_MAIN(tst_Storage)
#include "tst_jsonentitystorage.moc"
