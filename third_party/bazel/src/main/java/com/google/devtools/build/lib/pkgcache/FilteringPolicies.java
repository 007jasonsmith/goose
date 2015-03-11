// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
package com.google.devtools.build.lib.pkgcache;

import com.google.common.base.Preconditions;
import com.google.devtools.build.lib.packages.Rule;
import com.google.devtools.build.lib.packages.Target;
import com.google.devtools.build.lib.packages.TargetUtils;

import java.util.Objects;

/**
 * Utility class for predefined filtering policies.
 */
public final class FilteringPolicies {

  private FilteringPolicies() {
  }

  /**
   * Base class for singleton filtering policies.
   */
  private abstract static class AbstractFilteringPolicy implements FilteringPolicy {
    @Override
    public int hashCode() {
      return getClass().getSimpleName().hashCode();
    }

    @Override
    public boolean equals(Object obj) {
      if (obj == null) {
        return false;
      }
      if (obj == this) {
        return true;
      }
      return getClass().equals(obj.getClass());
    }
  }

  private static class NoFilter extends AbstractFilteringPolicy {
    @Override
    public boolean shouldRetain(Target target, boolean explicit) {
      return true;
    }
  }

  public static final FilteringPolicy NO_FILTER = new NoFilter();

  private static class FilterManualAndObsolete extends AbstractFilteringPolicy {
    @Override
    public boolean shouldRetain(Target target, boolean explicit) {
      return explicit || !(TargetUtils.hasManualTag(target) || TargetUtils.isObsolete(target));
    }
  }

  public static final FilteringPolicy FILTER_MANUAL_AND_OBSOLETE = new FilterManualAndObsolete();

  private static class FilterTests extends AbstractFilteringPolicy {
    @Override
    public boolean shouldRetain(Target target, boolean explicit) {
      return TargetUtils.isTestOrTestSuiteRule(target)
          && FILTER_MANUAL_AND_OBSOLETE.shouldRetain(target, explicit);
    }
  }

  public static final FilteringPolicy FILTER_TESTS = new FilterTests();

  private static class RulesOnly extends AbstractFilteringPolicy {
    @Override
    public boolean shouldRetain(Target target, boolean explicit) {
      return target instanceof Rule;
    }
  }

  public static final FilteringPolicy RULES_ONLY = new RulesOnly();

  /**
   * Returns the result of applying y, if target passes x.
   */
  public static FilteringPolicy and(final FilteringPolicy x,
                                    final FilteringPolicy y) {
    return new AndFilteringPolicy(x, y);
  }

  private static class AndFilteringPolicy implements FilteringPolicy {
    private final FilteringPolicy firstPolicy;
    private final FilteringPolicy secondPolicy;

    public AndFilteringPolicy(FilteringPolicy firstPolicy, FilteringPolicy secondPolicy) {
      this.firstPolicy = Preconditions.checkNotNull(firstPolicy);
      this.secondPolicy = Preconditions.checkNotNull(secondPolicy);
    }

    @Override
    public boolean shouldRetain(Target target, boolean explicit) {
      return firstPolicy.shouldRetain(target, explicit)
          && secondPolicy.shouldRetain(target, explicit);
    }

    @Override
    public int hashCode() {
      return Objects.hash(firstPolicy, secondPolicy);
    }

    @Override
    public boolean equals(Object obj) {
      if (!(obj instanceof AndFilteringPolicy)) {
        return false;
      }
      AndFilteringPolicy other = (AndFilteringPolicy) obj;
      return other.firstPolicy.equals(firstPolicy) && other.secondPolicy.equals(secondPolicy);
    }
  }
}