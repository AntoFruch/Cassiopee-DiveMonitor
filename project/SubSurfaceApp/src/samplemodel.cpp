#include "samplemodel.h"
#include "divecomputerwrapper.h"

void SampleModel::setEntries(const int diveId) {
    beginResetModel();
    m_entries = DiveDatabase::instance().getDiveEntries(diveId);
    endResetModel();
}

QVariant SampleModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_entries.size()) {
        return QVariant();
    }

    const int row = index.row();
    const auto &e = m_entries.at(row);

    switch (role) {
    case TimeRole:  return e.time;
    case DepthRole: return e.depth;
    case TempRole:  return e.temperature;

    case VelocityRole: {
        // Sécurité : au moins 2 points requis pour un calcul de vitesse
        if (m_entries.size() < 2) {
            return 0.0;
        }

        double velocity = 0.0;

        // Note : On utilise .toDouble() au cas où .depth ou .time sont des QVariants
        // Si ce sont déjà des double/int dans ta structure, le cast reste sécurisé.

        if (row > 0 && row < m_entries.size() - 1) {
            // --- Formule à 3 points (Différence centrée) ---
            const auto &prev = m_entries.at(row - 1);
            const auto &next = m_entries.at(row + 1);

            double dt = QVariant(next.time).toDouble() - QVariant(prev.time).toDouble();
            if (dt > 0) {
                double dy = QVariant(next.depth).toDouble() - QVariant(prev.depth).toDouble();
                velocity = dy / dt;
            }
        }
        else if (row == 0) {
            // Premier point : Différence vers l'avant (forward difference)
            const auto &next = m_entries.at(1);
            double dt = QVariant(next.time).toDouble() - QVariant(e.time).toDouble();
            if (dt > 0) {
                double dy = QVariant(next.depth).toDouble() - QVariant(e.depth).toDouble();
                velocity = dy / dt;
            }
        }
        else {
            // Dernier point : Différence vers l'arrière (backward difference)
            const auto &prev = m_entries.at(row - 1);
            double dt = QVariant(e.time).toDouble() - QVariant(prev.time).toDouble();
            if (dt > 0) {
                double dy = QVariant(e.depth).toDouble() - QVariant(prev.depth).toDouble();
                velocity = dy / dt;
            }
        }

        return velocity; // Retourne un double encapsulé dans un QVariant
    }

    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SampleModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TimeRole] = "time";
    roles[DepthRole] = "depth";
    roles[TempRole] = "temp";
    roles[VelocityRole] = "velocity";
    return roles;
}