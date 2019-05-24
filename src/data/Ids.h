#ifndef MILK_IDS_H
#define MILK_IDS_H

#include <string>
#include <queue>
#include <vector>

#include "data/int.h"

namespace milk {
	struct IdData {
		std::queue<U16> freeIndeces;
		std::vector<U16> generations;
	};

	namespace id {
		const static U32 INVALID;

		U32 create(IdData& ids);
		void remove(IdData& ids, U32 id);
		bool valid(IdData& ids, U32 id);
	}
}

#endif