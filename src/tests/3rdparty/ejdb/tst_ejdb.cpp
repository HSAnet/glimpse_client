#include <QtTest>

#include <QDebug>
#include <tcejdb/ejdb.h>

class TestEjdb : public QObject
{
    Q_OBJECT

private slots:
    // Code taken from https://github.com/Softmotions/ejdb#one-snippet-intro-6 and
    // slightly modified
    void demo()
    {
        EJDB *jb = ejdbnew();
        QCOMPARE(ejdbopen(jb, "addressbook", JBOWRITER | JBOCREAT | JBOTRUNC), 1);

        //Get or create collection 'contacts'
        EJCOLL *coll = ejdbcreatecoll(jb, "contacts", NULL);
        QVERIFY(coll != NULL);

        bson bsrec;
        bson_oid_t oid;

        //Insert one record:
        //JSON: {'name' : 'Bruce', 'phone' : '333-222-333', 'age' : 58}
        bson_init(&bsrec);
        bson_append_string(&bsrec, "name", "Bruce");
        bson_append_string(&bsrec, "phone", "333-222-333");
        bson_append_int(&bsrec, "age", 58);
        bson_finish(&bsrec);

        QCOMPARE(bsrec.err, 0);

        //Save BSON
        QCOMPARE(ejdbsavebson(coll, &bsrec, &oid), 1);
        qDebug() << "Saved Bruce";
        bson_destroy(&bsrec);

        //Now execute query
        //QUERY: {'name' : {'$begin' : 'Bru'}} //Name starts with 'Bru' string
        bson bq1;
        bson_init_as_query(&bq1);
        bson_append_start_object(&bq1, "name");
        bson_append_string(&bq1, "$begin", "Bru");
        bson_append_finish_object(&bq1);
        bson_finish(&bq1);

        EJQ *q1 = ejdbcreatequery(jb, &bq1, NULL, 0, NULL);

        uint32_t count;
        TCLIST *res = ejdbqryexecute(coll, q1, &count, 0, NULL);
        fprintf(stderr, "\n\nRecords found: %d\n", count);

        QCOMPARE(count, (uint32_t)1);

        //Now print the result set records
        for (int i = 0; i < TCLISTNUM(res); ++i) {
            void *bsdata = TCLISTVALPTR(res, i);
            bson_print_raw((const char*)bsdata, 0);
        }
        fprintf(stderr, "\n");

        //Dispose result set
        tclistdel(res);

        //Dispose query
        ejdbquerydel(q1);
        bson_destroy(&bq1);

        //Close database
        ejdbclose(jb);
        ejdbdel(jb);
    }
};

QTEST_MAIN(TestEjdb)

#include "tst_ejdb.moc"
