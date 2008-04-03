// -*-c++-*-

/*!
	\file disp_holder.cpp
	\brief display data holder class Source File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.	If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disp_holder.h"

/*-------------------------------------------------------------------*/
/*!

*/
DispHolder::DispHolder()
    : M_log_version( 0 )
{

}

/*-------------------------------------------------------------------*/
/*!

*/
DispHolder::~DispHolder()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::clear()
{
    M_playmode = rcss::rcg::PM_Null;
    M_teams[0].clear();
    M_teams[1].clear();

    M_score_changed_index.clear();
    M_penalty_scores_left.clear();
    M_penalty_scores_right.clear();

    M_last_disp.reset();
    M_dispinfo_cont.clear();

    M_server_param = rcss::rcg::ServerParamT();
    M_player_param = rcss::rcg::PlayerParamT();
    M_player_types.clear();
}

/*-------------------------------------------------------------------*/
/*!

*/
DispConstPtr
DispHolder::getDispInfo( const std::size_t idx ) const
{
    if ( M_dispinfo_cont.size() <= idx )
    {
        return DispConstPtr(); // null pointer
    }

    return M_dispinfo_cont[idx];
}

namespace {

struct TimeCmp {
    bool operator()( const DispPtr & lhs,
                     const int time ) const
      {
          return lhs->show_.time_ < time;
      }
};

}

/*-------------------------------------------------------------------*/
/*!

*/
std::size_t
DispHolder::getIndexOf( const int time ) const
{
    std::vector< DispPtr >::const_iterator it
        = std::lower_bound( M_dispinfo_cont.begin(),
                            M_dispinfo_cont.end(),
                            time,
                            TimeCmp() );
    if ( it == M_dispinfo_cont.end() )
    {
        return 0;
    }

    return std::distance( M_dispinfo_cont.begin(), it );
}

/*-------------------------------------------------------------------*/
/*!

*/
const
rcss::rcg::PlayerTypeT &
DispHolder::playerType( const int id ) const
{
    std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it
        = M_player_types.find( id );

    if ( it == M_player_types.end() )
    {
        return M_default_player_type;
    }

    return it->second;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleLogVersion( int ver )
{
    M_log_version = ver;
}

/*-------------------------------------------------------------------*/
/*!

*/
int
DispHolder::doGetLogVersion() const
{
    return M_log_version;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleShowInfo( const rcss::rcg::ShowInfoT & show )
{
    DispPtr disp( new rcss::rcg::DispInfoT );

    disp->pmode_ = M_playmode;
    disp->team_[0] = M_teams[0];
    disp->team_[1] = M_teams[1];
    disp->show_ = show;

    M_last_disp = disp;
    M_dispinfo_cont.push_back( disp );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleMsgInfo( const int,
                             const std::string & msg )
{
    if ( ! msg.compare( 0, std::strlen( "(team_graphic_" ), "(team_graphic_" ) )
    {
        // TODO
        // analyzeTeamGraphic( msg );
        return;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandlePlayMode( const rcss::rcg::PlayMode pmode )
{
    if ( pmode == rcss::rcg::PM_PenaltyScore_Left
         || pmode == rcss::rcg::PM_PenaltyMiss_Right )
    {
        int cycle = ( M_last_disp
                      ? M_last_disp->show_.time_
                      : 0 );
        M_penalty_scores_left.push_back( std::make_pair( cycle, pmode ) );
    }
    else if ( pmode == rcss::rcg::PM_PenaltyScore_Right
              || pmode == rcss::rcg::PM_PenaltyMiss_Right )
    {
        int cycle = ( M_last_disp
                      ? M_last_disp->show_.time_
                      : 0 );
        M_penalty_scores_right.push_back( std::make_pair( cycle, pmode ) );
    }

    M_playmode = pmode;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleTeamInfo( const rcss::rcg::TeamT & team_l,
                              const rcss::rcg::TeamT & team_r )
{
    if ( M_teams[0].score_ != team_l.score_
         || M_teams[1].score_ != team_r.score_ )
    {
        M_score_changed_index.push_back( M_dispinfo_cont.size() );
    }

    M_teams[0] = team_l;
    M_teams[1] = team_r;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleServerParam( const rcss::rcg::ServerParamT & param )
{
    M_server_param = param;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandlePlayerParam( const rcss::rcg::PlayerParamT & param )
{
    M_player_param = param;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandlePlayerType( const rcss::rcg::PlayerTypeT & param )
{
    M_player_types.insert( std::pair< int, rcss::rcg::PlayerTypeT >( param.id_, param ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DispHolder::doHandleEOF()
{

}