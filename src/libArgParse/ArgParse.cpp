// Copyright 2019 IBM Corporation
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <libArgParse/ArgParse.hpp>

//uint32_t ArgParse::GrammarElement::m_instanceCounter = 0;

std::string ArgParse::ParsedElement::getDebugString(const std::string & f_prefix)
{
    std::string result;
    if(m_grammarElement == nullptr)
    {
        return "!!Uninitialized Element!!";
    }
    result += f_prefix + m_grammarElement->getTypeName() +
        "(" + m_grammarElement->getElementName() + "/"  + m_grammarElement->getTag()+ "): "+
        " matched string: \"" + getMatchedString() + "\" " +
        " document: \"" + m_grammarElement->getDocument() + "\" " +
        "(" + std::string(m_stops ? "stopped" : "alive") + ")\n";
    for(auto child : m_children)
    {
        result += child->getDebugString(f_prefix + "  ");
    }
    return result;
}

std::string ArgParse::ParsedElement::findFirstChild(const std::string & f_elementName, uint32_t f_depth)
{
    bool found = false;
    ParsedElement & result = findFirstSubTree(f_elementName, found, f_depth);
    if(found)
    {
        return result.getMatchedString();
    }
    else
    {
        return "";
    }
}

std::string ArgParse::ParsedElement::getShortDocument() const
{
    std::string rightmostDoc = "";
    for(auto iChild = m_children.rbegin(); iChild != m_children.rend(); iChild++)
    {
        std::string doc = (*iChild)->getShortDocument();
        if( doc != "")
        {
            return doc;
        }
    }

    return m_grammarElement->getDocument();
}

void ArgParse::ParsedElement::findAllSubTrees(const std::string & f_elementName, std::vector<ArgParse::ParsedElement *> & f_out_result, bool f_doNotSearchChildsOfMatchingElements, uint32_t f_depth)
{
    if(m_grammarElement->getElementName() == f_elementName)
    {
        f_out_result.push_back(this);
        if(f_doNotSearchChildsOfMatchingElements)
        {
            return;
        }
    }

    if(f_depth == 0)
    {
        return;
    }

    for(auto child : m_children)
    {
        child->findAllSubTrees(f_elementName, f_out_result, f_doNotSearchChildsOfMatchingElements, f_depth - 1);
    }
}

ArgParse::ParsedElement & ArgParse::ParsedElement::findFirstSubTree(const std::string & f_elementName, bool & f_out_found, uint32_t f_depth)
{
    //std::cout << "searching for " << f_elementName << " going through " << m_grammarElement->getTypeName() << " " << m_grammarElement->getElementName() << std::endl;
    if(m_grammarElement->getElementName() == f_elementName)
    {
        f_out_found = true;
        //std::cout << "searched for " << f_elementName << " returning true\n";
        return *this;
    }
    else if(f_depth == 0)
    {
        f_out_found = false;
        return *this;
    }
    else
    {
        for(auto child : m_children)
        {
            bool found = false;
            ParsedElement & result = child->findFirstSubTree(f_elementName, found, f_depth - 1);
            if(found)
            {
                f_out_found = true;
                return result;
            }
        }
    }

    f_out_found = false;
    return *this;
}

std::string ArgParse::Grammar::getDotGraph()
{
    //std::cout << "GENERATING DOT GRAPH\n";
    std::string result = "digraph {\n";
    result += "ordering=out;\n";
    for(auto & node : m_nodes)
    {
        //printf("getting info for node %p\n", node.get());
        result += node->getDotNode();
    }
    result += "}\n";
    return result;
}

// not used, only for formatting the options().DebugString() of protobuf reflection
std::string ArgParse::ParsedDocument::getOptionString(std::string f_optString)
{
    std::string::size_type pos_eq = f_optString.find_first_of(':');
    std::string output;
    if (pos_eq != std::string::npos)
    {
        output = f_optString.substr(pos_eq+1);
    }
    output.erase(remove(output.begin(), output.end(), '"' ), output.end());
    for(size_t i =0; i<output.length(); i++)
    {
        if(output[i] == '\n')
        {
            output[i] = ' ';
        }
    }
    std::string delims = "\r\n\t ";
    output.erase(0, output.find_first_not_of(delims));
    output.erase(output.find_last_not_of(delims) + 1);

    return output;
}
