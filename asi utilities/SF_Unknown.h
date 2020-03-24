#pragma once


inline int ABS(int x) {
	return x > 0 ? x : -x;
}


namespace ASI
{
	#pragma pack(1)
	struct SF_PositionQuery
	{
		unsigned short x;
		unsigned short y;
		unsigned char other_data[4];

		inline SF_PositionQuery(void* ptr)
		{
			x = ((unsigned short*)(ptr))[0];
			y = ((unsigned short*)(ptr))[1];
			other_data[0] = ((unsigned char*)(ptr))[4];
			other_data[1] = ((unsigned char*)(ptr))[5];
			other_data[2] = ((unsigned char*)(ptr))[6];
			other_data[3] = ((unsigned char*)(ptr))[7];
		}
	};

	inline unsigned short FastDistance(SF_PositionQuery& p1, SF_PositionQuery& p2)
	{
		auto val8 = (p2.x) - (p1.x);    // x difference?
		auto val9 = ABS(val8);    // bigger of deltas

		auto val10 = (p2.y) - (p1.y);
		auto val11 = ABS(val10);  // smaller of deltas

		if (val11 > val9)
		{
			auto tmp_swap = val11;
			val11 = val9;
			val9 = tmp_swap;
		}

		return ((val11 * 13) / 32 + val9);
	}

	#pragma pack(1)
	struct SF_Unk1
	{
		unsigned char c1;
		unsigned short s1;
		unsigned short s2;
		unsigned short s3;

		SF_Unk1()
		{
			c1 = 0;
			s1 = 0;
			s2 = 0;
			s3 = 0;
		}

		SF_Unk1(unsigned char _c1, unsigned short _s1, unsigned short _s2, unsigned short _s3)
		{
			c1 = _c1;
			s1 = _s1;
			s2 = _s2;
			s3 = _s3;
		}

		SF_Unk1(void* ptr)
		{
			c1 = ((unsigned char*)ptr)[0];
			s1 = ((unsigned short*)((unsigned int)ptr + 1))[0];
			s2 = ((unsigned short*)((unsigned int)ptr + 1))[1];
			s3 = ((unsigned short*)((unsigned int)ptr + 1))[2];
		}
	};
}