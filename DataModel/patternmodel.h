#ifndef PATTERNMODEL_H
#define PATTERNMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "pointerconverter.h"
#include <QDeclarativeItem>
#include "../DSP/pattern.h"
#include <QMetaType>

//Warning : completely thread unsafe. When removing a pattern, do HWLock first!
class PatternModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PatternDataRole{
        PatternRole = Qt::UserRole + 1,
        NameRole,
        SessionRole
    };
    explicit PatternModel(QObject *parent = 0);
    ~PatternModel();

    //Read interfaces
    int rowCount(const QModelIndex &parent = QModelIndex())const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //Edit interfaces
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    //Add/remove interfaces
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    void import(PatternModel*);
    int count(){ return patternList.count();}

    Q_INVOKABLE void setData(int row, const QString& role, QVariant data)
    {
        int roleID=roleNames().key(role.toLocal8Bit());
        setData(index(row),data,roleID);
    }
    Pattern* Get(int index){
        return patternList[index];
    }

    Pattern* recordNewPattern();
    //Storage structures
    QList<Pattern*> patternList;
public slots:
    void patternDeleted(Pattern* p);

private:
    int nextPatternID;
};

Q_DECLARE_METATYPE(PatternModel*)
#endif // PATTERNMODEL_H
