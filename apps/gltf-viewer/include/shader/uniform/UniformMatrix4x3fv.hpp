#ifndef OPENGL_UNIFORMMATRIX43_HPP
#define OPENGL_UNIFORMMATRIX43_HPP

#include <shader/uniform/IUniform.hpp>


namespace shader {
    
    class UniformMatrix4x3fv : public IUniform {
        
        public:
            
            using IUniform::IUniform;
            
            void load(const void *value) final;
    };
}

#endif //OPENGL_UNIFORMMATRIX43_HPP
