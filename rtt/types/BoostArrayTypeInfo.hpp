/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  BoostArrayTypeInfo.hpp

                        BoostArrayTypeInfo.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_TEMPLATE_BOOSTARRAY_INFO_HPP
#define ORO_TEMPLATE_BOOSTARRAY_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "../internal/ArrayPartDataSource.hpp"
#include "type_discovery.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

namespace RTT
{
    namespace types
    {
        /**
         * Template for data types that are of type boost::array<U,int>
         *
         * @param T A boost::array<U> wrapper, where U is a data type.
         */
        template<typename T, bool has_ostream = false>
        class BoostArrayTypeInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            BoostArrayTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            virtual std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                std::vector<std::string> result;
                result.push_back("size");
                result.push_back("capacity");
                return result;
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                using namespace internal;
                typename AssignableDataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< AssignableDataSource<T> >( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // size and capacity can not change during program execution:
                if (name == "size" || name == "capacity") {
                    return new ConstantDataSource<int>( T::static_size );
                }

                // contents of indx can change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return new ArrayPartDataSource<typename T::value_type>( *data->set().c_array(), new ConstantDataSource<unsigned int>(indx), item, T::static_size);
                } catch(...) {}
                log(Error) << "BoostArrayTypeInfo: No such part (or invalid index): " << name << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                using namespace internal;
                typename AssignableDataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< AssignableDataSource<T> >( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // discover if user gave us a part name or index:
                typename DataSource<unsigned int>::shared_ptr id_indx = DataSource<unsigned int>::narrow( id.get() );
                typename DataSource<string>::shared_ptr id_name = DataSource<string>::narrow( id.get() );
                if ( id_name ) {
                    // size and capacity can not change during program execution:
                    if (id_name->get() == "size" || id_name->get() == "capacity") {
                        return new ConstantDataSource<int>( T::static_size );
                    }
                }

                if ( id_indx ) {
                    return new ArrayPartDataSource<typename T::value_type>( *data->set().c_array(), id_indx, item, T::static_size );
                }
                log(Error) << "BoostArrayTypeInfo: No such part (or invalid index): " << id_name->get() << id_indx->get() << endlog();
                return base::DataSourceBase::shared_ptr();
            }
            /**
             * Uses Decomposition to update result from source.
             */
            virtual bool composeTypeImpl(const PropertyBag& source, typename internal::AssignableDataSource<T>::reference_t result) const
            {
                //result.resize( source.size() );
                if(result.size() != source.size()) {
                    log(Error) << "Refusing to compose Boost Arrays from a property list of different size. Use the same number of properties as the C++ boost array size." << endlog();
                    return false;
                }
                // recurse into items of this sequence:
                PropertyBag target( source.getType() );
                PropertyBag decomp;
                internal::ReferenceDataSource<T> rds(result);
                rds.ref(); // prevent dealloc.
                // we compose each item in this sequence and then update result with target's result.
                // 1. each child is composed into target (this is a recursive thing)
                // 2. we decompose result one-level deep and 'refresh' it with the composed children of step 1.
                if ( composePropertyBag(source, target) && typeDecomposition( &rds, decomp, false) && ( decomp.getType() == target.getType() ) && refreshProperties(decomp, target, true) ) {
                    assert(result.size() == source.size());
                    assert(source.size() == target.size());
                    assert(source.size() == decomp.size());
                    return true;
                }
                return false;
            }

        };
    }
}

#endif
