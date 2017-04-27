
#ifndef INFUINTSEQUENCE_H
#define INFUINTSEQUENCE_H

using namespace BitSequence_NS;

namespace InfIntSequence_NS {

	template <typename ENDIAN>
	class InfUintSequence {
		BitStream_NS::BitStream<ENDIAN> bseq;
		
	public:
		InfUintSequence(byte* const bit_first, byte* const bit_last);
		operator bool() const;
		
		bool skip_next();
		bool has_next() const;
		template <typename UINT>
		bool peek_next(UINT& value) const;
		
		
		// TODO must implement bits-remaining checks
		template <typename UINT>
		bool operator<<(const UINT& u) {
			UINT n;
			bool o;
			if (u < 3) {
				// Treat values <3 as having the most sig. bit
				//  at bit pos. CHAR_BIT*sizeof(UINT)
				n = CHAR_BIT*sizeof(UINT);
				o=false;
			} else {
				n = u;
				for (std::size_t i=1; i<CHAR_BIT*sizeof(UINT); i<<=1) {
					n |= n>>i;
				}
				n^=(n>>1);
				o = u & (n>>1);
				n = get_bit_pos(n);
			}
			if (!bseq.has_next(2*n+o-3)) return false;
			bseq.set(true,n+o-3);
			bseq << false;
			bseq << !o;
			bseq.set(u, n-2);
			
			return *this;
		}
		template <typename UINT>
		bool operator>>(UINT& u) {
			std::size_t n;
			bool o;
			n = bseq.get(true);
			bseq.skip_next();
			bseq >> o;
			
			u=0;
			bseq.get(u,n+o);
			u+=(UINT(3)+o)<<n;
		}
	};
}
#endif
