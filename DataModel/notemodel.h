#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "pointerconverter.h"
#include "../DSP/patternnote.h"
#include "../DSP/pattern.h"
#include <QDeclarativeItem>
#include <QMetaType>

/*
Since the note model will be used by a (most probably horizontal) list view,
we cannot control the offset of each item at our disposal. However, since all
items are queued, we know how far is the total length of the items before the
current one. We can then only calculate the *relative* position to the last one,
and then set our *padding* length to take up the space. The padding data is not
necessarily inserted into the note. The model will take care of it.

For example, if we have an arrangement like this:

 |-----1-----|    (space)  |-2-|      |-----------3-----------|
                                \____/
                                   |
                 The padding length of note 2

Another thing to notice is that, to display the correct position of the first
note, we must prepend a dummy note to the note list with no pattern, only do padding.
The PatternNoteDisplay should check, if pattern is null, then it's a dummy note.
*/

//Note : since the RT audio thread will make intense query on a notemodel object for samples,
//When doing addition/deletion/modification, we must have HWLOCK

class Track;
class NoteModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum NoteRoles{
        NameRole = Qt::UserRole + 1,
        PatternRole, //answer the pattern pointer with a qstring ( for the view to draw the pattern )
        PaddingRole, //for answering the padding property
        OffsetRole,  //for the start offset inside a pattern
        LengthRole   //How long is this pattern note (a pattern can be looped)

        //TODO other properties, such as color
    };
    explicit NoteModel(QObject *parent = 0);
    //Read interfaces
    int rowCount(const QModelIndex &parent = QModelIndex())const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //Edit interfaces
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    //Track arrangement interfaces
    Q_INVOKABLE void insertNote(QString patternToken, int position);
    Q_INVOKABLE void removeNote(QString patternToken, int position);
    Q_INVOKABLE void moveNote(QString patternToken, int oldPosition, int newPosition);
    Q_INVOKABLE void moveNoteHead(QString patternToken, int position, int delta);
    Q_INVOKABLE void moveNoteTail(QString patternToken, int position, int delta);

    friend class Track;

signals:
    
public slots:
    void patternDeleted(Pattern*);
    void onBPMChange(double oldBPM, double newBPM);//TODO

private :
    QList<PatternNote> noteList;
    void insertNote(PatternNote& note,int position);
    void moveNoteHead(int index, int delta);
    void moveNoteTail(int index, int delta);
    void removeNote(int index);
};

Q_DECLARE_METATYPE(NoteModel*)

#endif // NOTEMODEL_H
