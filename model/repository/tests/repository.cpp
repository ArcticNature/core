// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/model/repository.h"

using sf::core::exception::ServiceNotFound;

using sf::core::model::IStreamRef;
using sf::core::model::Repository;
using sf::core::model::RepositoryVersion;
using sf::core::model::RepositoryVersionRef;


class TestVersion : public RepositoryVersion {
 protected:
  // Known files.
  static const std::set<std::string> FILES;

 public:
  bool exists(std::string path) {
    return this->FILES.find(path) != this->FILES.end();
  }
  
  IStreamRef streamFile(const std::string path) {
    return IStreamRef();
  }
};
const std::set<std::string> TestVersion::FILES = {
  "services/definition/test.service",
  "services/internal/application.service"
};


class TestRepo : public Repository {
 public:
  RepositoryVersionRef fixed_ver;
  RepositoryVersionRef latest_ver;

  TestRepo() {
    this->fixed_ver  = RepositoryVersionRef(new TestVersion());
    this->latest_ver = RepositoryVersionRef(new TestVersion());
  }

  RepositoryVersionRef latest() {
    return this->latest_ver;
  }

  RepositoryVersionRef version(std::string version) {
    return this->fixed_ver;
  }

  std::string resolveVersion(const std::string version) {
    return version;
  }

  void verifyVersion(const std::string version) {
    // Noop.
  }
};


TEST(RepositoryVersion, DefinitionFileForService) {
  TestVersion version;
  ASSERT_EQ(
      "services/definition/test.service",
      version.findDefinitionFile("definition.test")
  );
}

TEST(RepositoryVersion, DefinitionFileForParent) {
  TestVersion version;
  ASSERT_EQ(
      "services/internal/application.service",
      version.findDefinitionFile("internal.application.web.nginx")
  );
}

TEST(RepositoryVersion, DefinitionFileNotFound) {
  TestVersion version;
  ASSERT_THROW(
      version.findDefinitionFile("not.a.valid.service"),
      ServiceNotFound
  );
}


TEST(Repository, FindsLatest) {
  TestRepo repo;
  ASSERT_EQ(repo.latest_ver, repo.latest());
}

TEST(Repository, FindsVersion) {
  TestRepo repo;
  ASSERT_EQ(repo.fixed_ver, repo.version("0"));
}

TEST(Repository, LookupLatest) {
  TestRepo repo;
  ASSERT_EQ(repo.latest_ver, repo.lookup("<latest>"));
}

TEST(Repository, LookupVersion) {
  TestRepo repo;
  ASSERT_EQ(repo.fixed_ver, repo.lookup("0"));
}
