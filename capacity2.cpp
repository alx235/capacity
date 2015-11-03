//g++ capacity2.cpp -o capacity2 -Wall -pthread -std=c++0x
#include <iostream>
#include <string.h>
#include <vector>


namespace User {
/*COPY ON_WRITE AND REPRESENTATION VIEW
    struct Srep {

        char* s;
        int sz;//symbols count
        int n;//ref counter

        Srep(int nsz, const char* p) {
            n=1;
            sz = nsz;
            s = new char[sz+1];//add terminal-zero

            strcpy(s,p);
        }
        
        ~Srep() {delete[] s;}

        Srep* get_own_copy() {
            if (n==1) return this;
            n--;
            return new Srep(sz,s);
        }

        void assign (int nsz, const char* p) {
            if (sz!=nsz) {
                delete[] s;
                sz = nsz;
                s= new char [sz+1];
            }
            strcpy(s, p);
        }
    private://block copy and assignment operations
        Srep(const Srep&);
        Srep& operator=(const Srep&);
    };

    class String {

        //ref to String, work with s[i]
        //As for me, we can use only char* c and save memory))
        class Cref {
            friend class String;//access to private String member

            String& s;
            int i;

            Cref(String& str_ref, int i):s(str_ref),i(i) {}

            public:
            operator char() const {
                s.check(i);
                return s.read(i);
            }

            void operator= (char c) {//not check range!!!!
                s.write(i,c);
            }
        };

        Srep* rep;//representation

        void check (int i) const {
            if ((i<0) || (rep->sz<=i)) throw Range();
        }
        char read(int i) const {
            return rep->s[i];
        }
        void write (int i,char c) {
            rep = rep->get_own_copy();
            rep->s[i]=c;
        }

    public:
        //exception class
        class Range {
        public:
            Range() {
                std::cout<<"index out of range\n";
            }
        };

        String() {
            rep = new Srep(0,"");
        }
        
        String(const String& x) {
            x.rep->n++;
            rep = x.rep;
        }

        String& operator=(const String& x) {
            x.rep->n++;
            if (--rep->n==0)  delete rep;
            rep = x.rep;
            return *this;
        }

        String(const char* s) {
            rep = new Srep(strlen(s),s);
        }

        String& operator=(const char* s) {
            if (rep->n==1)
                rep->assign(strlen(s),s);
            else {
                rep->n--;
                rep = new Srep(strlen(s),s);
            }
            return *this;
        }

        Cref operator[] (int i) {
            check(i);
            return Cref(*this,i);
        }
        //for const char, maybe
        char operator[] (int i) const {
            check(i);
            return rep->s[i];
        }

        int size() const {
            return rep->sz;
        }
    };
}
*/

    struct Srep {

        char* s;
        int sz;//symbols count
        int n;//ref counter

        Srep(int nsz, const char* p) {
            n=1;
            sz = nsz;
            s = new char[sz+1];//add terminal-zero

            strcpy(s,p);
        }
        
        ~Srep() {delete[] s;}

        Srep* get_own_copy() {
            if (n==1) return this;
            n--;
            return new Srep(sz,s);
        }

        void assign (int nsz, const char* p) {
            if (sz!=nsz) {
                delete[] s;
                sz = nsz;
                s= new char [sz+1];
            }
            strcpy(s, p);
        }
    private://block copy and assignment operations
        Srep(const Srep&);
        Srep& operator=(const Srep&);
    };

    template<class C> class String {

        //ref to String, work with s[i]
        //As for me, we can use only char* c and save memory))
        class Cref {
            friend class String;//access to private String member

            String& s;
            int i;

            Cref(String& str_ref, int i):s(str_ref),i(i) {}

            public:
            operator char() const {
                s.check(i);
                return s.read(i);
            }

            void operator= (char c) {//not check range!!!!
                s.write(i,c);
            }
        };

        Srep* rep;//representation

        void check (int i) const {
            if ((i<0) || (rep->sz<=i)) throw Range();
        }
        char read(int i) const {
            return rep->s[i];
        }
        void write (int i,char c) {
            rep = rep->get_own_copy();
            rep->s[i]=c;
        }

    public:
        //exception class
        class Range {
        public:
            Range() {
                std::cout<<"index out of range\n";
            }
        };

        String() {
            rep = new Srep(0,"");
        }
        
        String(const String& x) {
            x.rep->n++;
            rep = x.rep;
        }

        String& operator=(const String& x) {
            x.rep->n++;
            if (--rep->n==0)  delete rep;
            rep = x.rep;
            return *this;
        }

        String(const C* s) {
            rep = new Srep(strlen(s),s);
        }

        String& operator=(const char* s) {
            if (rep->n==1)
                rep->assign(strlen(s),s);
            else {
                rep->n--;
                rep = new Srep(strlen(s),s);
            }
            return *this;
        }

        template <typename C1>
        friend std::ostream& operator<<(std::ostream& os, const User::String<C1>& x);

        template <typename C1>
        friend std::istream& operator>>(std::istream& is, User::String<C1>& x);

        Cref operator[] (int i) {
            check(i);
            return Cref(*this,i);
        }
        //for const char, maybe
        char operator[] (int i) const {
            check(i);
            return rep->s[i];
        }

        int size() const {
            return rep->sz;
        }
    };

    template <typename C1>
    std::ostream& operator<<(std::ostream& os, const User::String<C1>& x) {
        os.write(x.rep->s,x.rep->sz);
        return os;
    }

    static const int def_size = 100;

    template <typename C1>
    std::istream& operator>>(std::istream& is, User::String<C1>& x) {
        int size = x.rep->sz;
        int size_ = 0;
        char *s_ = x.rep->s;
        char *tmp;

        int i = 0;
        bool is_over = 0;//flag overflow s
        bool zero_size = (size==0);
        bool read_next = 1;

        while(read_next) {
            if ((size_>size)||zero_size) {//overflow case:
                size_ += def_size;
                try {
                    tmp = new char[size_+1];
                    strcpy(tmp, s_);
                }
                catch(...) {
                    delete[] tmp;
                    throw;//bad allocation
                }
                delete[] s_;
                s_ = tmp;
                is_over =1;
            }

            if(!(is>>s_[i]))//write and increment
                read_next = 0;
            ++size_;
            ++i;
        }
        if (!(x.rep->n==1))
           x.rep->n--;
        if (is_over) {//if overflow we must update rep
            x.rep->s = s_;
            x.rep->sz = size_;
        }
        return is;
    }
}


int main (int argc, char * argv[]) {

/*COPY ON_WRITE AND REPRESENTATION VIEW
    User::String str = "abc";
    char c = str[4];
*/
    User::String<char> buf;

    std::cin>>buf;
    std::cout<<buf<<"\n";
}
