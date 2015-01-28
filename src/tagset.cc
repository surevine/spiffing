/***

Copyright 2014 Dave Cridland
Copyright 2014 Surevine Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***/

#include <spiffing/tagset.h>
#include <spiffing/tag.h>
#include <spiffing/category.h>

using namespace Spiffing;

TagSet::TagSet(std::string const & id, std::string const & name)
: m_id{id}, m_name{name} {
}

void TagSet::addTag(std::shared_ptr<Tag> const & t) {
	m_tags[t->name()] = t;
}

void TagSet::addCategory(Tag const & tag, std::shared_ptr<Category> const & cat) {
	m_cats[std::make_pair(tag.tagType(), cat->lacv())] = cat;
}

void TagSet::addMarking(std::unique_ptr<Marking> marking) {
	m_marking = std::move(marking);
}
