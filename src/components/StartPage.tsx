interface Props {
  slide: boolean;
}

export default function StartPage({ slide }: Props) {
  return (
    <div className={`start-page ${slide ? 'slide' : ''}`}>
      <div className="text-container">
        <div>Simple</div>
        <div>Quicks</div>
      </div>
    </div>
  )
}