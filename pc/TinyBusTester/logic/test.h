#ifndef TEST_H
#define TEST_H

#include <QObject>
#include "referenceData.h"

class Connection;
class TestItemBase;
class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(Connection &connection, const ReferenceData &referenceData, QObject *parent = nullptr);

    QList<TestItemBase *> tests() const;

    void dataReceived(const QByteArray &data);
    void dataSend(const QByteArray &data);

    void run(void);
    void abort(void);
    bool running(void) const;


    void runNext(void);

    uint8_t address(void) const;
    const ReferenceData &referenceData() const;

signals:
    void changed(void);

public slots:
    void on_completed(void);

private:
    QList<TestItemBase*> _tests;
    Connection &_connection;
    const ReferenceData &_referenceData;

    bool _running = false;
    uint32_t _index;
};

#endif // TEST_H
