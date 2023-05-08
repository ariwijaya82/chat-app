import './style.scss';

import QuicksIcon from '../../assets/quicks.svg';
import InboxIcon from '../../assets/inbox.svg';
import TaskIcon from '../../assets/task.svg';
import Ellipsis from '../../assets/ellipse.svg';

interface Props {
  state: string;
  changeState: (state: string) => void;
}

export default function Quicks({ state, changeState }: Props) {
  return (
    <div className={`quicks-container ${state}`}>
      <img
        className={`main-icon`}
        src={QuicksIcon}
        alt="main-icon"
        onClick={(e) => {
          e.stopPropagation();
          changeState(state === 'open' ? 'close' : 'open')
        }}
      />
      <p className={`inbox-title`}>Inbox</p>
      <img
        className={`inbox-icon`}
        src={InboxIcon}
        alt="inbox-icon"
        onClick={(e) => {
          e.stopPropagation();
          changeState('inbox');
        }}
      />
      <p className={`task-title`}>Task</p>
      <img
        className={`task-icon`}
        src={TaskIcon}
        alt="task-icon"
        onClick={(e) => {
          e.stopPropagation();
          changeState('task');
        }}
      />
      <img
        className={`shadow-icon`}
        src={Ellipsis}
        alt="shadow-icon"
      />
    </div>
  )
}