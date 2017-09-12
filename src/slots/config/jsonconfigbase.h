#ifndef JSONCONFIGBASE_H
#define JSONCONFIGBASE_H

#include <util/common_define.h>
#include <util/file_util.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

BEGIN_NAMESPACE(slots)

#define GET_MEMBER_ITR_IN_DOC(doc, itr, mem)            \
  itr = doc.FindMember(mem.c_str());                    \
  if (itr == doc.MemberEnd()) {                         \
    CLOG(ERROR) << "Member [" << mem << "] required.";  \
    return false;                                       \
  }


#define GET_MEMBER_IN_DOC(doc, itr, mem, dest)          \
  GET_MEMBER_ITR_IN_DOC(doc, itr, mem);                 \
  rapidjson::Value &dest = itr->value;

#define GET_INT32_MEMBER_IN_DOC(doc, itr, mem, dest)    \
  GET_MEMBER_ITR_IN_DOC(doc, itr, mem)                  \
  dest = itr->value.GetInt();

#define CHECK_MEMBER_IS_ARRAY(value, tag)               \
  if (!value.IsArray()) {                               \
    CLOG(ERROR) << "["<< tag <<"] should be array.";    \
    return false;                                       \
  }

namespace JsonConfigStr{
  const std::string sID = "ID";
  const std::string sWeight = "Weight";
  const std::string sReward = "Reward";
  const std::string sRewardType = "RewardType";
  const std::string sType = "Type";
};

class JsonConfigBase{
public:
  JsonConfigBase() {}
  ~JsonConfigBase() {}

  bool initFromJsonFile(const std::string &path) {
    return cgserver::processFile(
        path, std::bind(
            &JsonConfigBase::parseJsonFile, this, std::placeholders::_1));
  }

protected:
  bool parseJsonFile(const char *str) {
    rapidjson::Document doc;
    doc.Parse<0>(str);
    if (doc.HasParseError()) {
        CLOG(ERROR) << "parse config failed:" << doc.GetParseError();
        return false;
    }
    if (doc.IsArray() && doc.Size() == 0) {
        CLOG(ERROR) << "no obj in config file";
        return false;
    }
    return parseJsonDoc(doc);
  }

  virtual bool parseJsonDoc(rapidjson::Document &doc) = 0;
};

END_NAMESPACE
#endif
